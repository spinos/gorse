/*
 *  BoxOps.cpp
 *  vachellia
 *
 *  2019/5/10
 */

#include "BoxOps.h"
#include <qt_graph/GlyphConnection.h>
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

    m_outOps.sendImpulse();
}

bool BoxOps::hasInstance() const
{ return true; }

void BoxOps::connectTo(GlyphOps *another, const std::string &portName, GlyphConnection *line)
{
    m_outOps.append(line);
}

void BoxOps::disconnectFrom(GlyphOps *another, const std::string &portName, GlyphConnection *line)
{
    m_outOps.remove(line);
}

}
