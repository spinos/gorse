/*
 *  HorizonOps.cpp
 *  vachellia
 *
 */

#include "HorizonOps.h"
#include <interface/RenderableScene.h>

namespace alo {
   
HorizonOps::HorizonOps() :
m_planetRadius(6378100.f)
{
}

HorizonOps::~HorizonOps()
{}

void HorizonOps::addRenderableTo(RenderableScene *scene)
{
    setRenderableScene(scene);
    scene->enqueueCreateRenderable(this, opsId());
}
  
void HorizonOps::update()
{
    getFloatAttribValue(m_planetRadius, "r");
}

}
