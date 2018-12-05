/*
 *  AcaciaScene.cpp
 *  
 *
 *  Created by jian zhang on 3/30/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#include "AcaciaScene.h"
#include "ops/AllOps.h"
#include <qt_graph/GlyphItem.h>
#include <qt_ogl/CameraEvent.h>
#include <math/AFrustum.h>

using namespace alo;

AcaciaScene::AcaciaScene(GroupCollection<QJsonObject> *collector, QObject *parent)
    : GlyphScene(collector, parent)
{}

AcaciaScene::~AcaciaScene()
{}

GlyphOps *AcaciaScene::createOps(const QJsonObject &content)
{ 
    int k = content["id"].toInt();
    switch(k) {
    case AcaciaOpsType::AoRoot :
        return new RootOps;
    case AcaciaOpsType::AoMeshListener :
        return new MeshListenerOps;
    case AcaciaOpsType::AoEdgeCollapseTest :
        return new EdgeCollapseTest;
    case AcaciaOpsType::AoLodMeshIn :
        return new LodMeshIn;
    case AcaciaOpsType::AoPVSTest :
        return new PVSTest;
    default:
        break;
    }
    return new GlyphOps; 
}

void AcaciaScene::postCreation(GlyphItem *item)
{
	GlyphOps *op = item->ops();
	if(op->hasDrawable()) { 
		DrawableOps *dop = static_cast<DrawableOps *>(op);
		dop->addDrawableTo(this);
	}
    emit sendUpdateDrawable();
}

void AcaciaScene::preDestruction(GlyphItem *item)
{
    GlyphOps *op = item->ops();
    if(op->hasDrawable()) { 
        DrawableOps *dop = static_cast<DrawableOps *>(op);
        dop->removeDrawableFromScene();
    }
    delete op;
    emit sendUpdateDrawable();
}

void AcaciaScene::recvCameraChanged(const CameraEvent &x)
{
    GlyphDataType *block = firstGlyph();
    while(block) {
        for(int i=0;i<block->count();++i) {
            GlyphItem *g = block->value(i);
            GlyphOps *op = g->ops();
            op->recvCameraChanged(x);
        }
        block = nextGlyph(block);
    }
    if(x.progressEnd())
        emit sendUpdateDrawable();
}
