/*
 *  GlyphScene.h
 *  
 *
 *  Created by jian zhang on 3/30/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_GLYPH_SCENE_H
#define ALO_GLYPH_SCENE_H

#include <math/Float4.h>
#include <rng/Uniform.h>
#include <rng/Lehmer.h>
#include <sdb/L2Tree.h>
#include <QGraphicsScene>
#include <QJsonObject>

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
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
	
	virtual void onItemStateChanged() = 0;
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
	GlyphDataType *firstGlyph();
	GlyphDataType *nextGlyph(const GlyphDataType *x);
/// generate a unique id for ops of type
	virtual int getUid(const int typeId) = 0;
	
	bool glyphExists(const int i);

private:
	
private:
/// only one can be active
	GlyphItem *m_activeGlyph;
	QList<GlyphItem *> m_selectedGlyph;;
	GroupCollection<QJsonObject> *m_collector;
	sdb::L2Tree<int, GlyphItem *, 128, 128> m_glyphMap;
	Uniform<Lehmer> *m_rng;
	
};

}

#endif