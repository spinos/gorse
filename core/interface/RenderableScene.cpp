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

namespace alo {

RenderableScene::RenderableScene()
{
	RenderableCoordinateSystem *b = new RenderableCoordinateSystem;
	b->setDrawId(1);
	enqueueCreateDrawable(b, 0);
}

bool RenderableScene::intersectRay(const Ray& aray, IntersectResult& result)
{ 
	result.rayDistance() = 1e9f;
	bool hasIntersection = false;
	bool isFinished = false;

	ObjectDataType *block = m_drawQueue.begin();
    while(block) {
    	for (int i=0;i<block->count();++i) { 
            RenderableObjectState &it = block->value(i);

            if(it._state <= stHidden)
                continue;

            if(it._object->intersectRay(aray, result))
            	hasIntersection = true;

            if(it._state == stOverlay) {
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
{ memset(col, 0, 16); }

const EnvLightTyp* RenderableScene::environmentLight() const
{ return 0; }

void RenderableScene::enqueueCreateDrawable(RenderableObject* d, int groupId)
{
	RenderableObjectState a;
    a._state = stNormal;
    a._object = d;
    m_drawQueue.insert(sdb::Coord2(d->drawId(), groupId), a);
}

void RenderableScene::enqueueHideDrawable(int objectId, int groupId)
{
	RenderableObjectState *a = m_drawQueue.find(sdb::Coord2(objectId, groupId) );
    if(a) a->_state = stHidden;
}

void RenderableScene::enqueueShowDrawable(int objectId, int groupId)
{
	RenderableObjectState *a = m_drawQueue.find(sdb::Coord2(objectId, groupId) );
    if(a) a->_state = stNormal;
}

void RenderableScene::enqueueRemoveDrawable(int objectId, int groupId)
{
	RenderableObjectState *a = m_drawQueue.find(sdb::Coord2(objectId, groupId) );
    if(a) a->_state = stWaitDestroy;
}

void RenderableScene::enqueueRemoveDrawable(int groupId)
{
	ObjectIteratorType it = m_drawQueue.begin(sdb::Coord2(-1,groupId));
    for(;!it.done();it.next()) {
        if(it.first.y > groupId) break;
        if(it.first.y < groupId) continue;
        if(it.second->_state > stWaitDestroy) it.second->_state = stWaitDestroy;
    }
}

void RenderableScene::enqueueShowDrawable(int groupId)
{
	ObjectIteratorType it = m_drawQueue.begin(sdb::Coord2(-1,groupId));
    for(;!it.done();it.next()) {
        if(it.first.y > groupId) break;
        if(it.first.y < groupId) continue;
        if(it.second->_state > stWaitDestroy) it.second->_state = stNormal;
    }
}

void RenderableScene::enqueueHideDrawable(int groupId)
{
	ObjectIteratorType it = m_drawQueue.begin(sdb::Coord2(-1,groupId));
    for(;!it.done();it.next()) {
        if(it.first.y > groupId) break;
        if(it.first.y < groupId) continue;
        if(it.second->_state > stWaitDestroy) it.second->_state = stHidden;
    }
}

}
