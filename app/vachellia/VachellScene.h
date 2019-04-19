/*
 *  VachellScene.h
 *  vachellia
 *
 *  Created by jian zhang on 4/11/19.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef VCHL_VACHELL_SCENE_H
#define VCHL_VACHELL_SCENE_H

#include <qt_graph/GlyphScene.h>
#include <interface/RenderableScene.h>

namespace alo {
class GlyphOps;
class CameraEvent;
class Hexahedron;
}

class VachellScene : public alo::GlyphScene, public alo::RenderableScene
{
    Q_OBJECT

public:
	VachellScene(alo::GroupCollection<QJsonObject> *collector, QObject *parent = 0);
	virtual ~VachellScene();
	
	virtual void onItemVisibilityChanged() override;

signals:
	void sendUpdateDrawable();
	void sendBound(const alo::Hexahedron &x);

public slots:
    void recvAttribChanged();
	void recvCameraChanged(const alo::CameraEvent &x);
	void recvRequestBound();
	void recvPreRenderRestart();
		
protected:
    virtual alo::GlyphOps *createOps(const QJsonObject &content) override;
    virtual void postCreation(alo::GlyphItem *item) override;
    virtual void preDestruction(alo::GlyphItem *item) override;
	
private:
	alo::GlyphOps *m_garbageOp;
};

#endif