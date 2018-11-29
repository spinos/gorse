#include "DrawableOps.h"
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>
#include <qt_ogl/DrawableResource.h>
#include <geom/V1ATriangleMesh.h>

namespace alo {

DrawableOps::DrawableOps() : m_drawCounter(0)
{}

DrawableOps::~DrawableOps()
{
	std::vector<DrawableResource *>::iterator it = m_resources.begin();
	for(;it!=m_resources.end();++it) {
		if(*it) delete *it;
	}
}

bool DrawableOps::hasDrawable() const
{ return true; }

void DrawableOps::setDrawable(DrawableObject *x, int i)
{ 
	if(m_drawables.size() < i+1)
		m_drawables.resize(i+1, NULL);
	m_drawables[i] = x; 
}

DrawableObject *DrawableOps::drawable(int i)
{ return m_drawables[i]; }

void DrawableOps::setDrawableScene(DrawableScene *x)
{ m_scene = x; }

DrawableScene *DrawableOps::drawableScene()
{ return m_scene; }

void DrawableOps::removeDrawableFromScene()
{ 
	m_scene->lock();
	std::vector<DrawableObject *>::iterator it = m_drawables.begin();
	for(;it!=m_drawables.end();++it)
		m_scene->enqueueRemoveDrawable(*it); 
	m_scene->unlock();
}

DrawableObject *DrawableOps::createDrawable()
{
	DrawableObject *d = new DrawableObject;
	d->setDrawId(m_drawCounter++);
	return d;
}

DrawableResource *DrawableOps::createResource()
{ return new DrawableResource; }

void DrawableOps::setResource(DrawableResource *x, int i)
{
	if(m_resources.size() < i+1)
		m_resources.resize(i+1, NULL);
	m_resources[i] = x; 
}

DrawableResource *DrawableOps::resource(int i)
{ return m_resources[i]; }

void DrawableOps::updateMeshResouce(DrawableResource *rec, const ver1::ATriangleMesh *mesh, bool showUV)
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

}

DrawableObject *DrawableOps::setMeshDrawable(const ver1::ATriangleMesh *mesh, const DrawableResource *rec )
{
	DrawableObject *cly = createDrawable();
    updateDrawableResource(cly, rec, mesh->numIndices());
    return cly;
}

void DrawableOps::updateDrawableResource(DrawableObject *d, const DrawableResource *rec, int n)
{
	const SimpleBuffer<Vector3F> &posnml = rec->c_posnmlBuffer();
    const SimpleBuffer<Vector3F> &baryc = rec->c_barycBuffer();
    d->setPosnml((const float *)posnml.c_data(), posnml.capacityByteSize());
    d->setBarycentric((const float *)baryc.c_data(), baryc.capacityByteSize());
    d->setDrawArrayLength(n);
}

}
