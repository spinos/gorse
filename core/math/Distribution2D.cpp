/*
 *  Distribution2D.cpp
 *  aloe
 *
 *  Created by jian zhang on 3/30/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#include "Distribution2D.h"

namespace alo {

Distribution2D::Distribution2D()
{}

void Distribution2D::create(int nu, int nv)
{
	m_dim[0] = nu;
	m_dim[1] = nv;
	m_dim[2] = nu * nv;
	m_invDim[0] = 1.f / (float)nu;
	m_invDim[1] = 1.f / (float)nv;
/// nv-by-nu
	m_vpdf.create(nv, nu);
	m_vcdf.create(nv, nu);
	m_updf.create(nu);
	m_ucdf.create(nu);
}

void Distribution2D::compute()
{
	m_updf.setZero();
	const int& nu = m_dim[0];
	for(int i=0;i<nu;++i) {
/// each column
		computeVPdf(i);
		computeVCdf(i);
	}
	computeUPdf();
	computeUCdf();
}

void Distribution2D::computeVPdf(int i)
{
	const int& nv = m_dim[1];
/// sum of i-th column
	float& intergal = m_updf[i];
	float* vv = vpdf(i);
	for(int j=0;j<nv;++j) {
		intergal += vv[j];
	}
	
	const float oneOverIntergal = 1.f / intergal;
	for(int j=0;j<nv;++j) {
		vv[j] *= oneOverIntergal;
	}
}

void Distribution2D::computeVCdf(int i)
{
	const int& nv = m_dim[1];
	
	const float* pv = c_vpdf(i);
	float* cv = m_vcdf.column(i);
	cv[0] = pv[0];
	for(int j=1;j<nv;++j) {
		cv[j] = cv[j-1] + pv[j]; 
	}
}

void Distribution2D::computeUPdf()
{
	const int& nu = m_dim[0];
/// sum of all columns
	float intergal = 0;
	for(int i=0;i<nu;++i) {
		intergal += m_updf[i];
	}
	
	const float oneOverIntergal = 1.f / intergal;
	for(int i=0;i<nu;++i) {
		m_updf[i] *= oneOverIntergal;
	}
}

void Distribution2D::computeUCdf()
{
	const int& nu = m_dim[0];
	m_ucdf[0] = m_updf[0];
	for(int i=1;i<nu;++i) {
		m_ucdf[i] = m_ucdf[i-1] + m_updf[i];
	}
}

float* Distribution2D::vpdf(int i)
{ return m_vpdf.column(i); }

const float* Distribution2D::c_vpdf(int i) const
{ return m_vpdf.column(i); }

void Distribution2D::sample(float* uv, float& pdf, const float* r) const
{
	int iu = m_ucdf.lowerBound(r[0]) + 1;
	if(iu > m_dim[0] - 1) iu = m_dim[0] - 1;
	uv[0] = (float)iu * m_invDim[0];
	
	int iv = m_vcdf.lowerBound(r[1], iu) + 1;
	if(iv > m_dim[1] - 1) iv = m_dim[1] - 1;
	uv[1] = (float)iv * m_invDim[1];
	
	lookup(pdf, iu, iv);
}

const int& Distribution2D::uDim() const
{ return m_dim[0]; }

const int& Distribution2D::vDim() const
{ return m_dim[1]; }

void Distribution2D::lookup(float& pdf, const int& iu, const int& iv) const
{ pdf = m_updf[iu] * m_vpdf.column(iu)[iv] * m_dim[2]; }

}

