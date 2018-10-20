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
	void deselectGlyph();

	GlyphOps *getActiveOps() const;

signals:
	void sendSelectGlyph(bool x);
		
protected:
	virtual GlyphOps *createOps(const QJsonObject &content);
	virtual void postCreation(GlyphItem *item);
	
private:
	
private:
	GlyphItem *m_activeGlyph;
	QList<GlyphItem *> m_selectedGlyph;;
	GroupCollection<QJsonObject> *m_collector;
};

}

#endif