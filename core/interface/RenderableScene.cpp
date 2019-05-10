/*
 *  RenderableScene.cpp
 *  aloe
 *
 *  Created by zhang on 18-2-22.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#include "RenderableScene.h"
#include "RenderableObject.h"
#include <math/Ray.h>
#include "IntersectResult.h"
#include "RenderableSphere.h"
#include "RenderableBox.h"
#include "RenderableCapsule.h"
#include "RenderableCylinder.h"
#include "RenderableCone.h"
#include "RenderableCoordinateSystem.h"
#include "TranslateController.h"

namespace alo {

RenderableScene::RenderableScene() : m_objectCount(0),
m_changed(false)
{
	RenderableCoordinateSystem *b = new RenderableCoordinateSystem;
    b->setOverlay(true);
	createRenderable(b, 0);
    
    TranslateController *t = new TranslateController;
    t->setOverlay(true);
    t->setVisible(false);
    createRenderable(t, 1);
    
}

bool RenderableScene::intersectRay(const Ray& aray, IntersectResult& result) const
{ 
    aray.get(result.rayData());
	bool hasIntersection = false;
	bool isFinished = false;

	ObjectDataType *block = m_drawQueue.begin();
    while(block) {
    	for (int i=0;i<block->count();++i) { 
            RenderableObjectState &it = block->value(i);

            if(it._object->isHidden())
                continue;

            if(it._object->intersectRay(aray, result))
            	hasIntersection = true;

            if(hasIntersection && it._object->isOverlay()) {
            	isFinished = true;
            	break;
            }
        }
        if(isFinished) break;

        block = m_drawQueue.next(block);
    }

	return hasIntersection;
}

void RenderableScene::getBackgroundColor(float* col, const Vector3F& dir, SampleState* state) const
{ col[0] = .57f; col[1] = .57f; col[2] = .57f; col[3] = 0.f; }

const EnvLightTyp* RenderableScene::environmentLight() const
{ return 0; }

void RenderableScene::createRenderable(RenderableObject* d, int groupId)
{
    RenderableObjectState b;
    b._object = d;

    const int i = m_objectCount;
    m_objectCount++;

    b._object->setObjectId(i);

    m_drawQueue.insert(sdb::Coord2(i, groupId), b);
}

void RenderableScene::removeRenderable(int objectId, int groupId)
{
    sdb::Coord2 a(objectId, groupId);

    if(a.x > -1) {
        RenderableObjectState *b = m_drawQueue.find(a);
        if(b) b->_object->setToDestroy();
    }
    else {
        setToRemoveGroup(a.y);
    }

    compressQueue();

}

void RenderableScene::setToRemoveGroup(int groupId)
{
    ObjectIteratorType it = m_drawQueue.begin(sdb::Coord2(-1,groupId));
    for(;!it.done();it.next()) {
        if(it.first.y > groupId) break;
        if(it.first.y < groupId) continue;
        RenderableObject *obj = it.second->_object;
        if(obj) obj->setToDestroy();
    }
}

void RenderableScene::compressQueue()
{
    ObjectDataType *block = m_drawQueue.begin();
    while(block) {
        for (int i=0;i<block->count();++i) { 
            if(block->isSingular()) break;

            RenderableObjectState &it = block->value(i);

            if(it._object->isToDestroy()) {
                block->remove(block->key(i));
                i--;
            }
        }
        block = m_drawQueue.next(block);
    } 
}

bool RenderableScene::sceneChanged() const
{
    return m_changed;
}

void RenderableScene::updateScene()
{
    m_changed = false;
}

void RenderableScene::setSceneChanged()
{ m_changed = true; }

}
