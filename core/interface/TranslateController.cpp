/*
 *  TranslateController.cpp
 *  vachellia
 *
 */

#include "TranslateController.h"
#include "RenderableSphere.h"
#include "RenderableCylinder.h"
#include "RenderableCone.h"
#include "IntersectResult.h"

namespace alo {

TranslateController::TranslateController()
{
	m_sphere = new RenderableSphere;
	m_sphere->setRadius(.57f);

	m_axis[0] = new RenderableCylinder;
	m_axis[0]->setPoints(Vector3F(.63f, 0.f, 0.f), Vector3F(8.f, 0.f, 0.f));
	m_axis[0]->setRadius(.23f);

	m_axis[1] = new RenderableCylinder;
	m_axis[1]->setPoints(Vector3F(0.f, .63f, 0.f), Vector3F(0.f, 8.f, 0.f));
	m_axis[1]->setRadius(.23f);

	m_axis[2] = new RenderableCylinder;
	m_axis[2]->setPoints(Vector3F(0.f, 0.f ,.63f), Vector3F(0.f, 0.f, 8.f));
	m_axis[2]->setRadius(.23f);

	m_arrow[0] = new RenderableCone;
	m_arrow[0]->setPoints(Vector3F(8.f, 0.f, 0.f), Vector3F(10.f, 0.f, 0.f));
	m_arrow[0]->setRadius(.57f);

	m_arrow[1] = new RenderableCone;
	m_arrow[1]->setPoints(Vector3F(0.f, 8.f, 0.f), Vector3F(0.f, 10.f, 0.f));
	m_arrow[1]->setRadius(.57f);

	m_arrow[2] = new RenderableCone;
	m_arrow[2]->setPoints(Vector3F(0.f, 0.f, 8.f), Vector3F(0.f, 0.f, 10.f));
	m_arrow[2]->setRadius(.57f);
}

TranslateController::~TranslateController()
{
	delete m_sphere;
	delete m_axis[0];
	delete m_axis[1];
	delete m_axis[2];
	delete m_arrow[0];
	delete m_arrow[1];
	delete m_arrow[2];
}

bool TranslateController::intersectRay(const Ray& aray, IntersectResult& result) 
{ 
	bool stat = false;

	if(m_sphere->intersectRay(aray, result))
		stat = true;

	if(m_axis[0]->intersectRay(aray, result))
		stat = true;

	if(m_axis[1]->intersectRay(aray, result))
		stat = true;

	if(m_axis[2]->intersectRay(aray, result))
		stat = true;

	if(m_arrow[0]->intersectRay(aray, result))
		stat = true;

	if(m_arrow[1]->intersectRay(aray, result))
		stat = true;

	if(m_arrow[2]->intersectRay(aray, result))
		stat = true;

	return stat;
}

}
