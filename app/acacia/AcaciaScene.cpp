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
    default:
        break;
    }
    return new GlyphOps; 
}

