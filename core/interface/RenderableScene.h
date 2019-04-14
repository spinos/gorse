/*
 *  RenderableScene.h
 *  aloe
 *
 *  Created by zhang on 19-4-11.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_RENDERABLE_SCENE_H
#define ALO_RENDERABLE_SCENE_H

#include <QMutex>
#include "light.h"
#include <sdb/Types.h>
#include <sdb/L3Tree.h>

namespace alo {

class SampleState;
class RenderableObject;
class Ray;
class IntersectResult;

class RenderableScene {

	//QMutex m_mutex;

	enum ObjectState {
        stUnknown = 0,
        stWaitDestroy,
        stHidden,
        stNormal,
        stOverlay
    };

	struct RenderableObjectState {
        int _state;
        RenderableObject* _object;
    };

    sdb::L3Tree<sdb::Coord2, RenderableObjectState, 2048, 512, 1024 > m_drawQueue;
    typedef sdb::L3Node<sdb::Coord2, RenderableObjectState, 1024> ObjectDataType;
    typedef sdb::L3DataIterator<sdb::Coord2, RenderableObjectState, 1024> ObjectIteratorType;

public:

    RenderableScene();
    
    virtual bool intersectRay(const Ray& aray, IntersectResult& result);
    
	virtual void getBackgroundColor(float* col, const Vector3F& dir, SampleState* state) const;
	
	virtual const EnvLightTyp* environmentLight() const;

    void enqueueCreateDrawable(RenderableObject* d, int groupId);
    
    void enqueueHideDrawable(int objectId, int groupId);
    void enqueueShowDrawable(int objectId, int groupId);
    void enqueueRemoveDrawable(int objectId, int groupId);
/// remove entire group
    void enqueueRemoveDrawable(int groupId);
    void enqueueShowDrawable(int groupId);
    void enqueueHideDrawable(int groupId);
	
protected:

private:

};

}

#endif
