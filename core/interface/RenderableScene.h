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

#include <vector>
#include "light.h"
#include <sdb/Types.h>
#include <sdb/L3Tree.h>

namespace alo {

class SampleState;
class RenderableObject;
class Ray;
class IntersectResult;

class RenderableScene {

	int m_objectCount;
	bool m_changed;

	struct RenderableObjectState {
        RenderableObject* _object;
    };

    struct CreateRenderableObjectState {
        int _group;
        RenderableObject* _object;
    };

    std::vector<CreateRenderableObjectState> m_createQueue;
    std::vector<sdb::Coord2> m_removeQueue;

    sdb::L3Tree<sdb::Coord2, RenderableObjectState, 2048, 512, 1024 > m_drawQueue;
    typedef sdb::L3Node<sdb::Coord2, RenderableObjectState, 1024> ObjectDataType;
    typedef sdb::L3DataIterator<sdb::Coord2, RenderableObjectState, 1024> ObjectIteratorType;

public:

    RenderableScene();
    
    virtual bool intersectRay(const Ray& aray, IntersectResult& result);
    
	virtual void getBackgroundColor(float* col, const Vector3F& dir, SampleState* state) const;
	
	virtual const EnvLightTyp* environmentLight() const;

    void enqueueCreateRenderable(RenderableObject* d, int groupId);
    
/// remove entire group when objectId = -1
    void enqueueRemoveRenderable(int objectId, int groupId);
    
/// has create remove queue
    bool sceneChanged() const;
    void updateRenderQueue();
	
protected:
    void setSceneChanged();

private:
    void setToRemoveGroup(int groupId);
    void compressQueue();
    void processCreateRenderableQueue();
    void processRemoveRenderableQueue();
    
};

}

#endif
