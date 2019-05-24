/*
 *  RenderableScene.h
 *  aloe
 *
 *  2019/5/26
 */

#ifndef ALO_RENDERABLE_SCENE_H
#define ALO_RENDERABLE_SCENE_H

#include <string>
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

    std::string m_name;
	int m_objectCount;
    int m_changeCount;
	bool m_changed;
    bool m_inProgress;

	struct RenderableObjectState {
        RenderableObject* _object;
    };

    sdb::L3Tree<sdb::Coord2, RenderableObjectState, 2048, 512, 1024 > m_drawQueue;
    typedef sdb::L3Node<sdb::Coord2, RenderableObjectState, 1024> ObjectDataType;
    typedef sdb::L3DataIterator<sdb::Coord2, RenderableObjectState, 1024> ObjectIteratorType;

public:

    RenderableScene();
    virtual ~RenderableScene();
    
    virtual bool intersectRay(const Ray& aray, IntersectResult& result) const;
    
	virtual void getBackgroundColor(float* col, const Vector3F& dir, SampleState* state) const;
	
	virtual const EnvLightTyp* environmentLight() const;

    void createRenderable(RenderableObject* d, int groupId);
/// remove entire group when objectId = -1
    void removeRenderable(int objectId, int groupId);

    const bool &sceneChanged() const;
    void updateScene();

    const std::string &renderableSceneName() const;

    void beginProgress();
    void endProgress();
    const bool &isInProgress() const;
	
protected:
    void setSceneChanged();
    int countTypedObjects(const int groupId) const;
    const int &numChanges() const;
    void resetChangeCount();
    void resetRenderableScene();
    void setName(const std::string &x);

private:
    void setToRemoveGroup(int groupId);
    void compressQueue();
    
};

}

#endif
