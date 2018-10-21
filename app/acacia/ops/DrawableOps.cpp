#include "DrawableOps.h"
#include <qt_ogl/DrawableScene.h>

namespace alo {

DrawableOps::DrawableOps()
{}

DrawableOps::~DrawableOps()
{}

bool DrawableOps::hasDrawable() const
{ return true; }

void DrawableOps::setDrawable(DrawableObject *x)
{ m_drawable = x; }

DrawableObject *DrawableOps::drawable()
{ return m_drawable; }

void DrawableOps::setDrawableScene(DrawableScene *x)
{ m_scene = x; }

DrawableScene *DrawableOps::drawableScene()
{ return m_scene; }

void DrawableOps::removeDrawableFromScene()
{ m_scene->enqueueRemoveDrawable(m_drawable); }

}
