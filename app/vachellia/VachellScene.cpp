/*
 *  VachellScene.cpp
 *  acacia
 *
 *  Created by jian zhang on 3/30/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#include "VachellScene.h"
#include "ops/allOps.h"
#include <qt_graph/GlyphOps.h>
#include <qt_graph/GlyphItem.h>
#include <math/CameraEvent.h>
#include <math/AFrustum.h>
#include <math/Hexahedron.h>

using namespace alo;

VachellScene::VachellScene(GroupCollection<QJsonObject> *collector, QObject *parent)
    : GlyphScene(collector, parent)
{}

VachellScene::~VachellScene()
{}

GlyphOps *VachellScene::createOps(const QJsonObject &content)
{/*
    int k = content["id"].toInt();
    switch(k) {
    case AcaciaOpsType::AoSphere :
        return new SphereOps;
    case AcaciaOpsType::AoRoot :
        return new RootOps;
    case AcaciaOpsType::AoLodMesh :
        return new LodMeshOps;
    case AcaciaOpsType::AoStaticMesh :
        return new SimpleMeshOps;
    case AcaciaOpsType::AoEdgeCollapseTest :
        return new EdgeCollapseTest;
    case AcaciaOpsType::AoLodMeshIn :
        return new LodMeshInOps;
    case AcaciaOpsType::AoPVSTest :
        return new PVSTest;
    case AcaciaOpsType::AoInstancedTest :
        return new InstancedTest;
    case AcaciaOpsType::AoGeodesicDistanceTest :
        return new GeodesicDistanceTest;
    default:
        break;
    }*/
    return new GlyphOps; 
}

void VachellScene::postCreation(GlyphItem *item)
{
	GlyphOps *op = item->ops();
	if(op->hasDrawable()) { 
		//DrawableOps *dop = static_cast<DrawableOps *>(op);
		//dop->addDrawableTo(this);
	}
    emit sendUpdateDrawable();
}

void VachellScene::preDestruction(GlyphItem *item)
{
    GlyphOps *op = item->ops();
    if(op->hasDrawable()) { 
        //DrawableOps *dop = static_cast<DrawableOps *>(op);
        //dop->removeDrawableFromScene();
    }
    delete op;
    emit sendUpdateDrawable();
}

void VachellScene::recvCameraChanged(const CameraEvent &x)
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
}

void VachellScene::recvRequestBound()
{
    GlyphOps *op = getActiveOps();
    if(!op) return;
    if(!op->hasDrawable() ) return;
    Hexahedron b;
    op->getDrawableBound(b);
    emit sendBound(b);
}
