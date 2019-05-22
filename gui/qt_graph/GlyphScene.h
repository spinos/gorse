/*
 *  GlyphScene.h
 *  gorse
 *
 *  2019/5/20
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

	struct GlyphProfile {
		int _type;
		int _id;
		QPointF _pos;
		bool _isLoading;
	};

	void createGlyph(const GlyphProfile &param);
	void selectGlyph(GlyphItem *item);
	void deselectGlyph(GlyphItem *item);
	void deselectAllGlyph();
	void removeActiveGlyph();

	GlyphItem *getActiveGlyph() const;
	GlyphOps *getActiveOps() const;
	
	virtual void onItemStateChanged(GlyphItem *item, QGraphicsItem *stateControlItem) = 0;
	virtual void onFocusIn3D(const Float4 &centerRadius);
	virtual void createConnection(GlyphConnection *conn, GlyphPort *port) = 0;
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

	typedef sdb::L3Node<int, GlyphItem *, 128> GlyphDataType;
	GlyphDataType *firstGlyph() const;
	GlyphDataType *nextGlyph(const GlyphDataType *x) const;
/// generate a unique id for ops of type
	int getUid(const int typeId);
	
	bool glyphExists(const int i);
	
	QJsonObject getGlyphProfile(int typeId);

	void preLoad();
	void postLoad();

private:

	void resetGlyphScene();
	
private:
/// only one can be active
	GlyphItem *m_activeGlyph;
	QList<GlyphItem *> m_selectedGlyph;
/// glyph properties
	GroupCollection<QJsonObject> *m_collector;
	sdb::L2Tree<int, int, 64, 64> m_typeCounter;
	sdb::L2Tree<int, GlyphItem *, 128, 128> m_glyphMap;
	bool m_isLoading;

};

} /// end of alo

#endif