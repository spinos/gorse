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

void VisibleDetail::set(bool v, float d)
{
	int n = m_visible.count();
	for(int i=0;i<n;++i)
		m_visible[i] = v;
	for(int i=0;i<n;++i)
		m_detail[i] = d;
}

bool *VisibleDetail::visibilities()
{ return m_visible.data(); }

float *VisibleDetail::levelOfDetails()
{ return m_detail.data(); }

const bool *VisibleDetail::c_visibilities() const
{ return m_visible.c_data(); }

const float *VisibleDetail::c_levelOfDetails() const
{ return m_detail.c_data(); }

}
