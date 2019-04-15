/*
 *  VachellScene.cpp
 *  vachellia
 *
 *  Created by jian zhang on 4/16/19.
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
{
    int k = content["id"].toInt();
    switch(k) {
        case HorizonOps::Type :
            return new HorizonOps;
        default:
            break;
    }
    return new GlyphOps; 
}

void VachellScene::postCreation(GlyphItem *item)
{
	GlyphOps *op = item->ops();
	if(op->hasRenderable()) { 
		RenderableOps *dop = static_cast<RenderableOps *>(op);
		dop->addRenderableTo(this);
	}
    emit sendUpdateDrawable();
}

void VachellScene::preDestruction(GlyphItem *item)
{
    GlyphOps *op = item->ops();
    if(op->hasRenderable()) { 
        RenderableOps *dop = static_cast<RenderableOps *>(op);
        dop->removeRenderableFromScene();
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
