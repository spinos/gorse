#include "DrawableOps.h"
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>
#include <qt_ogl/DrawableResource.h>
#include <geom/V1ATriangleMesh.h>
#include <cull/VisibilityState.h>

namespace alo {

DrawableOps::DrawableOps() : m_drawCounter(0)
{}

DrawableOps::~DrawableOps()
{}

bool DrawableOps::hasDrawable() const
{ return true; }

void DrawableOps::setDrawableScene(DrawableScene *x)
{ m_scene = x; }

DrawableScene *DrawableOps::drawableScene()
{ return m_scene; }

void DrawableOps::removeDrawableFromScene()
{ 
	m_scene->lock();
	m_scene->enqueueRemoveDrawable(opsId() ); 
	m_scene->unlock();
}

DrawableObject *DrawableOps::createDrawable()
{
	DrawableObject *d = new DrawableObject;
	d->setDrawId(m_drawCounter++);
	return d;
}

void DrawableOps::UpdateMeshResouce(DrawableResource *rec, const ver1::ATriangleMesh *mesh, bool showUV)
{
	const int oldL = rec->size();

	if(showUV && mesh->numUVSets() > 0) 
        mesh->createUVNormalArray(rec->posnmlBuffer());
    else
    	mesh->createPositionNormalArray(rec->posnmlBuffer());

    const bool expanded = oldL < rec->size();
    rec->setToRelocate(expanded);
    if(expanded)
        mesh->createBarycentricCoordinates(rec->barycBuffer());

    rec->setDrawArrayLength(mesh->numIndices());
    rec->setDirty(true);
}

void DrawableOps::initiateResource(DrawableResource *rec)
{
    DrawableObject *d = createDrawable();
    rec->attachToDrawable(d);
    
    m_scene->lock();
    m_scene->enqueueCreateDrawable(d, opsId());
    m_scene->unlock();
}

void DrawableOps::processResource(DrawableResource *rec, bool forcedToRelocate)
{
    DrawableObject *d = rec->drawable();

    if(!d) {
        initiateResource(rec);
        return;
    }

    if(rec->toRelocate() || forcedToRelocate) {
        m_scene->lock();
        m_scene->enqueueRemoveDrawable(d->drawId(), opsId());
        m_scene->unlock();
        initiateResource(rec);
        return;
    } 

    if(rec->isDirty()) {
        d->setDrawArrayLength(rec->drawArrayLength());
        m_scene->lock();
        m_scene->enqueueEditDrawable(d->drawId(), opsId());
        m_scene->unlock();
        rec->setDirty(false);
    }
}

void DrawableOps::processResource(DrawableResource *rec, const VisibilityState &vis, bool forcedToRelocate)
{
    DrawableObject *d = rec->drawable();

    if(vis.isDormant() && d) {
        m_scene->lock();
        m_scene->enqueueRemoveDrawable(d->drawId(), opsId());
        m_scene->unlock();
        rec->dettachDrawable();
        return;
    }

    if(vis.isHidden() && d) {
        m_scene->lock();
        m_scene->enqueueHideDrawable(d->drawId(), opsId());
        m_scene->unlock();
        return;
    }

    if(!vis.isVisible())
        return;

    if(!d) {
        initiateResource(rec);
        return;
    }

    if(rec->toRelocate() || forcedToRelocate) {
        m_scene->lock();
        m_scene->enqueueRemoveDrawable(d->drawId(), opsId());
        m_scene->unlock();
        initiateResource(rec);
        return;
    } 

    if(rec->isDirty()) {
        d->setDrawArrayLength(rec->drawArrayLength());
        m_scene->lock();
        m_scene->enqueueEditDrawable(d->drawId(), opsId());
        m_scene->unlock();
        rec->setDirty(false);
    }

    if(vis.isStateChanged()) {
        m_scene->lock();
        m_scene->enqueueShowDrawable(d->drawId(), opsId());
        m_scene->unlock();
    }
}

}
