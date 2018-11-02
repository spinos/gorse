/*
 *  AcaciaScene.cpp
 *  
 *
 *  Created by jian zhang on 3/30/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

//#include <QtGui>
#include "AcaciaScene.h"
#include "ops/AllOps.h"
#include <qt_graph/GlyphItem.h>

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
    case AcaciaOpsType::AoEdgeCollapseTest :
        return new EdgeCollapseTest;
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
        emit sendUpdateDrawable();
	}
}

void AcaciaScene::preDestruction(GlyphItem *item)
{
    GlyphOps *op = item->ops();
    if(op->hasDrawable()) { 
        DrawableOps *dop = static_cast<DrawableOps *>(op);
        dop->removeDrawableFromScene();
        emit sendUpdateDrawable();
    }
    delete op;
}
