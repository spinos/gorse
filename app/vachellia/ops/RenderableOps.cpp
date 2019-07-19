/*
 *  RenderableOps.cpp
 *  vachellia
 *
 *  2019/5/26
 */

#include "RenderableOps.h"
#include <interface/RenderableScene.h>

namespace alo {

RenderableOps::RenderableOps()
{}

RenderableOps::~RenderableOps()
{}

bool RenderableOps::hasRenderable() const
{ return true; }

void RenderableOps::setDrawableVisible(bool x)
{ RenderableObject::setVisible(x); }

void RenderableOps::setRenderableScene(RenderableScene *x)
{ m_scene = x; }

void RenderableOps::removeRenderableFromScene()
{ 
	m_scene->removeRenderable(objectId(), opsTypeId() );
}

bool RenderableOps::getVisibleState() const
{
	return !RenderableObject::isHidden();
}

RenderableScene *RenderableOps::renderableScene()
{ return m_scene; }

const smp::SampleFilter<SurfaceGeodesicSample> *RenderableOps::getGeodesicSamples() const
{ return nullptr; }

} /// end of alo
