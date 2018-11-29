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

#include <rng/Uniform.h>
#include <rng/Lehmer.h>
#include <sdb/L3Tree.h>
#include <QGraphicsScene>
#include <QJsonObject>

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QPixmap;
QT_END_NAMESPACE

namespace alo {

class GlyphItem;
class GlyphOps;

template<typename T>
class GroupCollection;

class GlyphScene : public QGraphicsScene
{
    Q_OBJECT

public:
	GlyphScene(GroupCollection<QJsonObject> *collector, 
				QObject *parent = 0);
	virtual ~GlyphScene();

	void createGlyph(const QPixmap &pix, int typ, const QPointF & pos);
	void selectGlyph(GlyphItem *item);
	void deselectGlyph(GlyphItem *item);
	void deselectAllGlyph();
	void removeActiveItem();

	GlyphItem *getActiveGlyph() const;
	GlyphOps *getActiveOps() const;

signals:
	void sendSelectGlyph(bool x);
	
protected:
	virtual GlyphOps *createOps(const QJsonObject &content);
	virtual void postCreation(GlyphItem *item) = 0;
	virtual void preDestruction(GlyphItem *item) = 0;

	typedef sdb::L3Node<int, GlyphItem *, 128> GlyphDataType;
	GlyphDataType *firstGlyph();
	GlyphDataType *nextGlyph(const GlyphDataType *x);
	
private:
	int getUid();
	
private:
/// only one can be active
	GlyphItem *m_activeGlyph;
	QList<GlyphItem *> m_selectedGlyph;;
	GroupCollection<QJsonObject> *m_collector;
	sdb::L3Tree<int, GlyphItem *, 128, 128> m_glyphMap;
	Uniform<Lehmer> *m_rng;

};

}

#endif