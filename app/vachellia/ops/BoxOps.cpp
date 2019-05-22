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
#include <math/pointBox.h>

namespace alo {
   
BoxOps::BoxOps()
{
    m_boxSize[0] = 10.f;
    m_boxSize[1] = 10.f;
    m_boxSize[2] = 10.f;
}

BoxOps::~BoxOps()
{}

std::string BoxOps::opsName() const
{ return "box"; }

void BoxOps::addRenderableTo(RenderableScene *scene)
{
    setRenderableScene(scene);
    scene->createRenderable(this, opsTypeId());
}
  
void BoxOps::update()
{
    TransformOps::update();
    
    getFloatAttribValue(m_boxSize[0], "bx");
    getFloatAttribValue(m_boxSize[1], "by");
    getFloatAttribValue(m_boxSize[2], "bz");

    const float bx = m_boxSize[0] * .5f;
    const float by = m_boxSize[1] * .5f;
    const float bz = m_boxSize[2] * .5f;

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

QString BoxOps::getShortDescription() const
{
    return QString("width %1\nheight %2\ndepth %3").arg(QString::number(m_boxSize[0]), 
        QString::number(m_boxSize[1]), QString::number(m_boxSize[2])); 
}

} /// end of alo
