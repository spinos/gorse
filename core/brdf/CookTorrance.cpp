/*
 *  CookTorrance.cpp
 *  aloe
 *
 *  Created by jian zhang on 3/23/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#include "CookTorrance.h"
#include <math/SpaceTransform.h>

namespace alo {

CookTorrance::CookTorrance(const SpaceTransform* tm) : AbstractBrdf(tm)
{
	m_alpha = .2f;
	m_alpha2 = m_alpha * m_alpha;
	m_k = (m_alpha + 1) * (m_alpha + 1) * .125f;
	m_ior = 1.5f;
	computeR0();
}

float CookTorrance::computeFr(const Vector3F& wi) const
{
	float ndotl = wi.z;
	if(ndotl < 1e-3f) return 0;
	
	float vdoth;
	Vector3F hr = halfVector(wi, vdoth);
	const float& ndoth = hr.z;
	float ldoth = wi.dot(hr);
	const float& ndotv = wodotn();
	float D = microfacetDistribution(ndoth);
	float F = schlickFresnel(vdoth);
	float G = geometryTerm(vdoth, ldoth, ndotv, ndotl);
	return D*F*G / (ndotv * ndotl * 4.f + .01f);
}

/// pdf <- D * N.H / (4 * V.H ) 
/// s2013_pbs_epic_notes_v2
/// Real Shading in Unreal Engine 4 
/// pdf <- D * N.H
/// http://www.codinglabs.net/article_physically_based_rendering_cook_torrance.aspx
///	
float CookTorrance::computePdf(const Vector3F& wi) const
{
	float vdoth;
	Vector3F hr = halfVector(wi, vdoth);
	const float& ndoth = hr.z;
	if(ndoth < 1e-3f) return 1e-4f;
	float D = microfacetDistribution(ndoth);
	//float vdoth = m_wo.dot(hr);
	//return D * ndoth / (4.f * vdoth);
	return D * ndoth;
}

void CookTorrance::computeR0()
{
	m_R0 = (1.f - m_ior) / (1.f + m_ior);
	m_R0 = m_R0 * m_R0;
}

float CookTorrance::chiX(const float& x) const
{ 
	if(x > 0) return 1.f;
	return 0.f; 
}

float CookTorrance::microfacetDistribution(const float& ndoth) const
{
	float ndoth2 = ndoth * ndoth;
	float denom = ndoth2 * m_alpha2 - ndoth2 + 1.f;
	return (chiX(ndoth) * m_alpha2) * ONE_OVER_PIF / (denom * denom);
}

float CookTorrance::schlickFresnel(const float cosTheta) const
{ return m_R0 + (1.f - m_R0) * pow( 1.f - cosTheta, 5); }

float CookTorrance::geometryFunc(const float& vdoth, const float vdotn) const
{
	//float chi = chiX(vdoth / vdotn);
	//float vdoth2 = vdoth * vdoth;
	//float tan2 = (1.f - vdoth2) / vdoth2;
	//return (chi * 2.f) / (1.f + sqrt(1 + m_alpha2 * tan2) );
/// s2013_pbs_epic_notes_v2
	return vdotn / (vdotn * (1.f - m_k) + m_k);
}

float CookTorrance::geometryTerm(const float& odoth, const float& idoth,
					const float& odotn, const float idotn) const
{
	return geometryFunc(odoth, odotn) * geometryFunc(idoth, idotn);
}

}