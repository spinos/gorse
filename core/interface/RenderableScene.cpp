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
	enqueueCreateRenderable(b, 0);
    
    TranslateController *t = new TranslateController;
    t->setOverlay(true);
    t->setVisible(false);
    enqueueCreateRenderable(t, 1);
    
    processCreateRenderableQueue();
    
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

void RenderableScene::enqueueCreateRenderable(RenderableObject* d, int groupId)
{
	CreateRenderableObjectState a;
    a._group= groupId;
    a._object = d;
    m_createQueue.push_back(a);
}

void RenderableScene::enqueueRemoveRenderable(int objectId, int groupId)
{
    m_removeQueue.push_back(sdb::Coord2(objectId, groupId));
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

void RenderableScene::processCreateRenderableQueue()
{
    while(m_createQueue.size() > 0) {

        CreateRenderableObjectState &a = m_createQueue.front();

        RenderableObjectState b;
        b._object = a._object;

        const int i = m_objectCount;
        m_objectCount++;

        b._object->setObjectId(i);

        m_drawQueue.insert(sdb::Coord2(i, a._group), b);

        m_createQueue.erase(m_createQueue.begin());
    }
}

void RenderableScene::processRemoveRenderableQueue()
{
    while(m_removeQueue.size() > 0) {

        sdb::Coord2 &a = m_removeQueue.front();

        if(a.x > -1) {
            RenderableObjectState *b = m_drawQueue.find(a);
            if(b) b->_object->setToDestroy();
        }
        else {
            setToRemoveGroup(a.y);
        }

        m_removeQueue.erase(m_removeQueue.begin());
    }

    compressQueue();
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
    if(m_createQueue.size() > 0) return true;
    if(m_removeQueue.size() > 0) return true;
    return m_changed;
}

void RenderableScene::updateRenderQueue()
{
    processCreateRenderableQueue();
    processRemoveRenderableQueue();
    m_changed = false;
}

void RenderableScene::setSceneChanged()
{ m_changed = true; }

}
