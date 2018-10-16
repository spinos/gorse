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
	
	//GardenGlyph* lastSelectedGlyph();
	//const GardenGlyph* lastSelectedGlyph() const;
	//const aphid::ATriangleMesh* lastSelectedGeom() const;
	//void selectGlyph(GardenGlyph* gl);
	//void deselectGlyph();

signals:
	void sendSelectGlyph(bool x);
		
protected:
	virtual GlyphOps *createOps(const QJsonObject &content);
/// first (selected) ground piece
	//virtual GardenGlyph* getGround();
	
private:
	
private:
	//QList<GardenGlyph *> m_selectedGlyph;
	//GardenGlyph* m_lastSelectedGlyph;
	GroupCollection<QJsonObject> *m_collector;
};

}

#endif