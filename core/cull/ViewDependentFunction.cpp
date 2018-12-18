/*
 *  ViewDependentFunction.cpp
 *  aloe
 *
 */

#include "ViewDependentFunction.h"
#include "ViewFrustumCull.h"
#include "VisibleDetail.h"
#include <math/PerspectiveCamera.h>
#include <math/AFrustum.h>

namespace alo {

ViewDependentFunction::ViewDependentFunction() :
m_freezeView(false)
{
	m_culler = new ViewFrustumCull;
    m_details = new VisibleDetail;
}

ViewDependentFunction::~ViewDependentFunction()
{
	delete m_culler;
    delete m_details;
}

void ViewDependentFunction::setFreezeView(bool x)
{ m_freezeView = x; }

const bool &ViewDependentFunction::freezeView() const
{ return m_freezeView; }

ViewFrustumCull *ViewDependentFunction::culler()
{ return m_culler; }

VisibleDetail *ViewDependentFunction::details()
{ return m_details; }

const VisibilityState &ViewDependentFunction::visibility(int i) const
{ return m_details->c_visibilities()[i]; }

void ViewDependentFunction::initializeDetails()
{
	const int &n = m_culler->numPrimitives();
	m_details->create(n);
	for(int i=0;i<n;++i)
		m_details->setDetail(0, i);
	m_details->setVisible(true);
    m_details->setDeltaDistance(m_culler->getMeanSize() / 32.f);
}

bool ViewDependentFunction::updateView(const PerspectiveCamera &camera, const AFrustum &frustum)
{
	if(!m_details->updateView(camera)) return false;
	m_culler->compare(m_details->visibilities(), frustum);
	return true;
}

void ViewDependentFunction::clearBvh()
{ m_culler->clearBvh(); }

void ViewDependentFunction::addBvhPrimitive(const BVHPrimitive &x)
{ m_culler->addBvhPrimitive(x); }

void ViewDependentFunction::buildBvh()
{ m_culler->buildBvh(); }

}
