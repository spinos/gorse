/*
 *  GlyphScene.h
 *  gorse
 *
 *  2019/5/25
 */

#ifndef ALO_GLYPH_SCENE_H
#define ALO_GLYPH_SCENE_H

#include <math/Float4.h>
#include <sdb/L2Tree.h>
#include <QGraphicsScene>
#include <QJsonObject>

QT_BEGIN_NAMESPACE
class QPixmap;
QT_END_NAMESPACE

namespace alo {

class GlyphItem;
class GlyphOps;
class GlyphConnection;
class GlyphPort;

template<typename T>
class GroupCollection;

class GlyphScene : public QGraphicsScene
{
    Q_OBJECT

public:
	GlyphScene(QObject *parent = 0);
	virtual ~GlyphScene();

	void setAssetCollection(GroupCollection<QJsonObject> *x);
	void initializeGraphics();

	struct CreateGlyphParameter {
		int _type;
		int _id;
		QPointF _pos;
		bool _isLoading;
        GlyphOps *_ops;
	};

	void createGlyph(CreateGlyphParameter &param);
	void selectGlyph(GlyphItem *item);
	void deselectGlyph(GlyphItem *item);
	void deselectAllGlyph();
	void removeActiveGlyph();

	GlyphItem *getActiveGlyph() const;
	GlyphOps *getActiveOps() const;
	
	virtual void onItemStateChanged(GlyphItem *item, QGraphicsItem *stateControlItem) = 0;
	virtual void onFocusIn3D(const Float4 &centerRadius);
/// attempt to complete connection at port
	virtual bool makeConnection(GlyphConnection *conn, GlyphPort *port) = 0;
	virtual void removeConnection(GlyphConnection *conn) = 0;
	
signals:
	void sendGlyphIcon(const QPixmap &pix);
	void sendSelectGlyph(bool x);
	void sendFocusCameraOn(const Float4 &centerRadius);
	
protected:
	virtual GlyphOps *createOps(const QJsonObject &content) = 0;
	
	virtual void postCreation(GlyphItem *item) = 0;
	virtual void postCreationBlocked(GlyphItem *item) = 0;
	virtual void preDestruction(GlyphItem *item, const std::vector<GlyphConnection *> &connectionsToBreak) = 0;

/// generate a unique id for ops of type
	int getUid(const int typeId);
	
	bool glyphExists(const int i);
	
	QJsonObject getGlyphProfile(int typeId);
/// add x to connection map
/// assign unique id if id of x < 0   
    void mapConnection(GlyphConnection *x);
/// id of x <- -1
    void unmapConnection(GlyphConnection *x);
    
    sdb::L3DataIterator<int, GlyphConnection *, 128> connectionsBegin() const;
    sdb::L3DataIterator<int, GlyphItem *, 128> glyphsBegin() const;

	void preLoad();
	void postLoad();
/// connect node0.port0 to node1.port1
/// map result to uid
	bool connectNodes(const int &node0Id, const std::string &port0Name,
					const int &node1Id, const std::string &port1Name,
					const int &uid);

private:

	void resetGlyphScene();
/// vacant id no less than base
    int getConnectionUid(const int &base);
	
private:
/// only one can be active
	GlyphItem *m_activeGlyph;
	QList<GlyphItem *> m_selectedGlyph;
/// glyph property preset
	GroupCollection<QJsonObject> *m_collector;
	sdb::L2Tree<int, int, 64, 64> m_typeCounter;
	sdb::L2Tree<int, GlyphItem *, 128, 128> m_glyphMap;
    sdb::L2Tree<int, GlyphConnection *, 128, 128> m_connectionMap;
    typedef GlyphConnection* GlyphConnectionPtr;
    typedef GlyphItem* GlyphItemPtr;
	bool m_isLoading;

};

} /// end of alo

#endif