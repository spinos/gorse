/*
 *  AcaciaScene.h
 *  
 *
 *  Created by jian zhang on 3/30/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ACACIA_SCENE_H
#define ACACIA_SCENE_H

#include <qt_graph/GlyphScene.h>

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

class AcaciaScene : public alo::GlyphScene
{
    Q_OBJECT

public:
	AcaciaScene(alo::GroupCollection<QJsonObject> *collector, QObject *parent = 0);
	virtual ~AcaciaScene();

signals:
	void sendSelectGlyph(bool x);
		
protected:

private:

};

#endif