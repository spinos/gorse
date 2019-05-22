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

	void createGlyph(const QPixmap &pix, int typ, const QPointF & pos);
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
	virtual GlyphOps *createOps(const QJsonObject &content);
	
	virtual void postCreation(GlyphItem *item) = 0;
	virtual void preDestruction(GlyphItem *item, const std::vector<GlyphConnection *> &connectionsToBreak) = 0;

	typedef sdb::L3Node<int, GlyphItem *, 128> GlyphDataType;
	GlyphDataType *firstGlyph() const;
	GlyphDataType *nextGlyph(const GlyphDataType *x) const;
/// generate a unique id for ops of type
	int getUid(const int typeId);
	
	bool glyphExists(const int i);
	void resetGlyphScene();

	QJsonObject getGlyphProfile(int typeId);

private:
	
private:
/// only one can be active
	GlyphItem *m_activeGlyph;
	QList<GlyphItem *> m_selectedGlyph;
/// glyph properties
	GroupCollection<QJsonObject> *m_collector;
	sdb::L2Tree<int, int, 64, 64> m_typeCounter;
	sdb::L2Tree<int, GlyphItem *, 128, 128> m_glyphMap;
	
};

} /// end of alo

#endif