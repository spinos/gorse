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
{ m_scene->enqueueRemoveRenderable(objectId(), opsId() ); }

}
