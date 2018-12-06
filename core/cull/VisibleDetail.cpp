/*
 *  VisibleDetail.cpp
 *  aloe
 *
 *
 */

#include "VisibleDetail.h"
#include <math/BaseCamera.h>
namespace alo {

VisibleDetail::VisibleDetail() :
m_viewPoint(Vector3F(-1e9f,-1e9f,-1e9f)),
m_viewDirection(Vector3F(0.f,0.f,0.f)),
m_deltaD(1.f)
{}

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

bool VisibleDetail::updateView(const BaseCamera &cam)
{
	const Vector3F p = cam.eyePosition();
	const Vector3F d = cam.eyeDirection();

	bool pChanged = p.distanceTo(m_viewPoint) > m_deltaD;
	bool dChanged = d.dot(m_viewDirection) < .991f;

	if(pChanged) m_viewPoint = p;
	if(dChanged) m_viewDirection = d;
	return pChanged || dChanged;
}

}
