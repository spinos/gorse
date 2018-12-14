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

const int &DrawableOps::frameNumber() const
{ return m_scene->frameNumber(); }

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
    rec->setChangedOnFrame(frameNumber());
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
        d->setEditDrawArrayLength(rec->drawArrayLength());
        m_scene->lock();
        m_scene->enqueueEditDrawable(d->drawId(), opsId());
        m_scene->unlock();
        rec->setDirty(false);
    }
}

void DrawableOps::processResourceNoLock(DrawableResource *rec)
{
    DrawableObject *d = rec->drawable();

    if(!d) {
        d = createDrawable();
        rec->attachToDrawable(d);
        rec->setChangedOnFrame(frameNumber());
        m_scene->enqueueCreateDrawable(d, opsId());
        return;
    }

    if(rec->toRelocate()) {
        m_scene->enqueueRemoveDrawable(d->drawId(), opsId());
        rec->dettachDrawable();
        d = createDrawable();
        rec->attachToDrawable(d);
        rec->setChangedOnFrame(frameNumber());
        m_scene->enqueueCreateDrawable(d, opsId());
        return;
    }

    if(rec->isDirty()) {
        d->setEditDrawArrayLength(rec->drawArrayLength());
        m_scene->enqueueEditDrawable(d->drawId(), opsId());
        rec->setDirty(false);
        rec->setChangedOnFrame(frameNumber());
    }
}

void DrawableOps::processResource(DrawableResource *rec, const VisibilityState &vis)
{
    DrawableObject *d = rec->drawable();

    if(vis.isDormant() && d) {
        m_scene->enqueueRemoveDrawable(d->drawId(), opsId());
        rec->dettachDrawable();
        return;
    }

    if(vis.isHidden() && d) {
        m_scene->enqueueHideDrawable(d->drawId(), opsId());
        return;
    }

    if(!vis.isVisible()) {
        return;
    }

    if(!d) {
        d = createDrawable();
        rec->attachToDrawable(d);
        rec->setChangedOnFrame(frameNumber());
        m_scene->enqueueCreateDrawable(d, opsId());
        return;
    }

    if(rec->toRelocate()) {
        m_scene->enqueueRemoveDrawable(d->drawId(), opsId());
        rec->dettachDrawable();
        d = createDrawable();
        rec->attachToDrawable(d);
        rec->setChangedOnFrame(frameNumber());
        m_scene->enqueueCreateDrawable(d, opsId());
        return;
    } 

    if(rec->isDirty()) {
        d->setEditDrawArrayLength(rec->drawArrayLength());
        m_scene->enqueueEditDrawable(d->drawId(), opsId());
        rec->setDirty(false);
        rec->setChangedOnFrame(frameNumber());
    }

    if(vis.isStateChanged()) {
        m_scene->enqueueShowDrawable(d->drawId(), opsId());
    }

    rec->setDeferred(false);
}

void DrawableOps::setDrawableSize(int n)
{
    for(int i=0;i<n;++i) {
        if(!hasResource(i)) {
            DrawableResource *rec = createResource();
            setResource(rec, i);
        }
    }

    if(numResources() == n) return;

    m_scene->lock();
    while(numResources() > n) {
        DrawableResource *r = lastResource();
        DrawableObject *d = r->drawable();
        if(d) m_scene->enqueueRemoveDrawable(d->drawId(), opsId());
        
        removeLastResource();
    }
    m_scene->unlock();
}

void DrawableOps::lockScene()
{ m_scene->lock(); }

void DrawableOps::unlockScene()
{ m_scene->unlock(); }

}
