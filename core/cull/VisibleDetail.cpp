/*
 *  VisibleDetail.cpp
 *  aloe
 *
 *
 */

#include "VisibleDetail.h"
#include <math/PerspectiveCamera.h>
namespace alo {

VisibleDetail::VisibleDetail() :
m_deltaD(1.f)
{
	m_param._viewPoint.set(-1e9f,-1e9f,-1e9f);
	m_param._screenWidth = 0.f;
	m_param._viewDirection.setZero();
}

VisibleDetail::~VisibleDetail()
{}

void VisibleDetail::create(int n)
{ 
	m_visible.createBuffer(n); 
	m_detail.createBuffer(n); 
}

void VisibleDetail::setVisible(bool v)
{
	int n = m_visible.count();
	for(int i=0;i<n;++i)
		m_visible[i].setVisible(v);
}

void VisibleDetail::setDetail(float x)
{
	int n = m_detail.count();
	for(int i=0;i<n;++i)
		m_detail[i].set(x);
}

void VisibleDetail::setDetail(int x, int i)
{ m_detail[i].set(x); }

void VisibleDetail::setDeltaDistance(float x)
{ m_deltaD = x; }

VisibilityState *VisibleDetail::visibilities()
{ return m_visible.data(); }

LevelOfDetailSelect *VisibleDetail::levelOfDetails()
{ return m_detail.data(); }

const VisibilityState *VisibleDetail::c_visibilities() const
{ return m_visible.c_data(); }

const LevelOfDetailSelect *VisibleDetail::c_levelOfDetails() const
{ return m_detail.c_data(); }

bool VisibleDetail::updateView(const PerspectiveCamera &cam)
{
	const Vector3F p = cam.eyePosition();
	const Vector3F d = cam.eyeDirection();
	int s = cam.portWidth() - m_param._screenWidth;

	bool pChanged = p.distanceTo(m_param._viewPoint) > m_deltaD;
	bool dChanged = d.dot(m_param._viewDirection) < .991f;
	bool sChanged = (s < -10 || s > 10);

	if(pChanged) m_param._viewPoint = p;
	if(dChanged) m_param._viewDirection = d;
	if(sChanged) m_param._screenWidth = cam.portWidth();
	m_param._tanhfov = cam.tanhfov();
	return pChanged || dChanged || sChanged;
}

const LevelOfDetailParam &VisibleDetail::param() const
{ return m_param; }

}
