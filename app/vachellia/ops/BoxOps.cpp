/*
 *  BoxOps.cpp
 *  vachellia
 *
 *  2019/4/26
 */

#include "BoxOps.h"
#include <interface/RenderableScene.h>
#include <interface/IntersectResult.h>

namespace alo {
   
BoxOps::BoxOps()
{}

BoxOps::~BoxOps()
{}

std::string BoxOps::opsName() const
{ return "box"; }

void BoxOps::addRenderableTo(RenderableScene *scene)
{
    setRenderableScene(scene);
    scene->enqueueCreateRenderable(this, opsId());
}
  
void BoxOps::update()
{
    TransformOps::update();
    
    float bx, by, bz;
    getFloatAttribValue(bx, "bx");
    getFloatAttribValue(by, "by");
    getFloatAttribValue(bz, "bz");

    bx *= .5f;
    by *= .5f;
    bz *= .5f;

    setAabb(-bx, -by, -bz, bx, by, bz);

    m_outOps.update();
}

void BoxOps::connectTo(GlyphOps *another, const std::string &portName)
{
    RenderableOps *r = static_cast<RenderableOps *>(another);
    std::cout << "\n BoxOps " << this << " connectTo renderable " << r;
    m_outOps.append(r);
}

void BoxOps::disconnectFrom(GlyphOps *another, const std::string &portName)
{
    RenderableOps *r = static_cast<RenderableOps *>(another);
    m_outOps.remove(r);
    std::cout << "\n BoxOps " << this << " disconnectFrom renderable " << r;
}

}
