/*
 *  UnionOps.cpp
 *  vachellia
 *
 */

#include "UnionOps.h"
#include <interface/RenderableScene.h>
#include <interface/IntersectResult.h>

namespace alo {
   
   
UnionOps::UnionOps()
{
}

UnionOps::~UnionOps()
{
}

std::string UnionOps::opsName() const
{ return "union"; }

void UnionOps::addRenderableTo(RenderableScene *scene)
{
    setRenderableScene(scene);
    scene->enqueueCreateRenderable(this, opsId());
}
  
void UnionOps::update()
{
    TransformOps::update();
    
}

bool UnionOps::intersectRay(const Ray& aray, IntersectResult& result)
{
    return TransformOps::intersectRay(aray, result);
/*
    float rayData[8];
    result.copyRayData(rayData);

    rayToLocal(rayData);

    if(!rayAabbIntersect(rayData, aabb())) return false;

    float &tt = rayData[6];
    const float &tLimit = rayData[7];
    float q[3];

    for(int i=0;i<256;++i) {
        
        rayTravel(q, rayData);

        float d = m_rule->lookup((const float *)&q);

        if(d < 1e-4f) break;

        if(d < tt * 1e-5f) break;

        tt += d * .7f;
     
        if(tt > tLimit) return false;
    }

    Vector3F tn = m_rule->lookupNormal(q);

    rayToWorld(rayData);
    normalToWorld((float *)&tn);

    return result.updateRayDistance(tt, tn);*/
}

}
