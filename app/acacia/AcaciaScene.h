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
#include <qt_ogl/DrawableScene.h>

namespace alo {
class GlyphOps;
class CameraEvent;
class Hexahedron;
}

class AcaciaScene : public alo::GlyphScene, public alo::DrawableScene
{
    Q_OBJECT

public:
	AcaciaScene(alo::GroupCollection<QJsonObject> *collector, QObject *parent = 0);
	virtual ~AcaciaScene();
	
	virtual void onItemVisibilityChanged() override;

signals:
	void sendUpdateDrawable();
	void sendBound(const alo::Hexahedron &x);

public slots:
	void recvCameraChanged(const alo::CameraEvent &x);
	void recvRequestBound();
		
protected:
    virtual alo::GlyphOps *createOps(const QJsonObject &content) override;
    virtual void postCreation(alo::GlyphItem *item) override;
    virtual void preDestruction(alo::GlyphItem *item) override;
	
private:

};

#endif