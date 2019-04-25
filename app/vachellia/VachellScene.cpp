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
#include <qt_graph/GlyphConnection.h>
#include <math/CameraEvent.h>
#include <math/AFrustum.h>
#include <math/Hexahedron.h>
#include <QDebug>

using namespace alo;

VachellScene::VachellScene(QObject *parent)
    : GlyphScene(parent)
{ m_garbageOp = nullptr; }

VachellScene::~VachellScene()
{}

GlyphOps *VachellScene::createOps(const QJsonObject &content)
{
    int k = content["id"].toInt();
    switch(k) {
        case BoxOps::Type :
            return new BoxOps;
        case HorizonOps::Type :
            return new HorizonOps;
        case VoxelOps::Type :
            return new VoxelOps;
        case RepeatOps::Type :
            return new RepeatOps;
        case UnionOps::Type :
            return new UnionOps;
        default:
            ;
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
    op->preDestruction();
    m_garbageOp = op;
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

void VachellScene::onItemVisibilityChanged()
{
	RenderableScene::setSceneChanged();
	emit sendUpdateDrawable();
}

void VachellScene::recvAttribChanged()
{
    RenderableScene::setSceneChanged();
	emit sendUpdateDrawable();
}

void VachellScene::recvPreRenderRestart()
{
    if(m_garbageOp) {
        delete m_garbageOp;
        m_garbageOp = nullptr;
    } 
}

void VachellScene::createConnection(GlyphConnection *conn, GlyphPort *port)
{
    if(!conn->canConnectTo(port) ) return;
	
	conn->destinationTo(port);
    RenderableScene::setSceneChanged();
	emit sendUpdateDrawable();
}

void VachellScene::removeConnection(GlyphConnection *conn)
{
	conn->breakUp();
    RenderableScene::setSceneChanged();
	emit sendUpdateDrawable();
}
