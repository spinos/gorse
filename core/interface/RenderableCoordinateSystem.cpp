/*
 *  RenderableCoordinateSystem.cpp
 *  vachellia
 *
 */

#include "RenderableCoordinateSystem.h"
#include "RenderableSphere.h"
#include "RenderableCylinder.h"
#include "RenderableCone.h"
#include "IntersectResult.h"

namespace alo {

RenderableCoordinateSystem::RenderableCoordinateSystem()
{
	m_sphere = new RenderableSphere;
	m_sphere->setRadius(.5f);

	m_axis[0] = new RenderableCylinder;
	m_axis[0]->setPoints(Vector3F::Zero, Vector3F(8.f, 0.f, 0.f));
	m_axis[0]->setRadius(.17f);

	m_axis[1] = new RenderableCylinder;
	m_axis[1]->setPoints(Vector3F::Zero, Vector3F(0.f, 8.f, 0.f));
	m_axis[1]->setRadius(.17f);

	m_axis[2] = new RenderableCylinder;
	m_axis[2]->setPoints(Vector3F::Zero, Vector3F(0.f, 0.f, 8.f));
	m_axis[2]->setRadius(.17f);

	m_arrow[0] = new RenderableCone;
	m_arrow[0]->setPoints(Vector3F(8.f, 0.f, 0.f), Vector3F(10.f, 0.f, 0.f));
	m_arrow[0]->setRadius(.5f);

	m_arrow[1] = new RenderableCone;
	m_arrow[1]->setPoints(Vector3F(0.f, 8.f, 0.f), Vector3F(0.f, 10.f, 0.f));
	m_arrow[1]->setRadius(.5f);

	m_arrow[2] = new RenderableCone;
	m_arrow[2]->setPoints(Vector3F(0.f, 0.f, 8.f), Vector3F(0.f, 0.f, 10.f));
	m_arrow[2]->setRadius(.5f);
}

RenderableCoordinateSystem::~RenderableCoordinateSystem()
{
	delete m_sphere;
	delete m_axis[0];
	delete m_axis[1];
	delete m_axis[2];
	delete m_arrow[0];
	delete m_arrow[1];
	delete m_arrow[2];
}

bool RenderableCoordinateSystem::intersectRay(IntersectResult& result) const
{ 
	bool stat = false;

	if(m_sphere->intersectRay(result))
		stat = true;

	if(m_axis[0]->intersectRay(result))
		stat = true;

	if(m_axis[1]->intersectRay(result))
		stat = true;

	if(m_axis[2]->intersectRay(result))
		stat = true;

	if(m_arrow[0]->intersectRay(result))
		stat = true;

	if(m_arrow[1]->intersectRay(result))
		stat = true;

	if(m_arrow[2]->intersectRay(result))
		stat = true;

	return stat;
}

}
