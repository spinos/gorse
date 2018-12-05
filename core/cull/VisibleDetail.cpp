/*
 *  VisibleDetail.cpp
 *  aloe
 *
 *
 */

#include "VisibleDetail.h"

namespace alo {

VisibleDetail::VisibleDetail()
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

VisibilityState *VisibleDetail::visibilities()
{ return m_visible.data(); }

LevelOfDetailSelect *VisibleDetail::levelOfDetails()
{ return m_detail.data(); }

const VisibilityState *VisibleDetail::c_visibilities() const
{ return m_visible.c_data(); }

const LevelOfDetailSelect *VisibleDetail::c_levelOfDetails() const
{ return m_detail.c_data(); }

}
