/*
 *  SampleState.cpp
 *  aloe img
 *
 *  Created by jian zhang on 3/26/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#include "SampleState.h"
#include <math/miscfuncs.h>
#include <iostream>

namespace alo {

SampleState::SampleState() : m_exposureWeight(1.f),
m_levelOfDetail(0.f),
m_rank(3)
{}

void SampleState::setAccess(AccessMode u, AccessMode v)
{ m_accessU = u; m_accessV = v; }

void SampleState::setFilter(FilterMode x)
{ m_filter = x; }

void SampleState::setChannel(ChannelMode x)
{ m_channel = x; }

void SampleState::setExposure(const float& x)
{ m_exposureWeight = pow(2.f, x + 2.47393f); }

void SampleState::setLevelOfDetail(const float& x)
{ m_levelOfDetail = x; }

float* SampleState::p()
{ return m_p; }

int* SampleState::u()
{ return m_u; }

int* SampleState::v()
{ return m_v; }

float* SampleState::buf(int corner)
{ return m_buf[corner]; }

float& SampleState::levelOfDetail()
{ return m_levelOfDetail; }

void SampleState::toLongitudeLatitude(const float* direction)
{ 	
	ToSphericalCoord(m_p, direction);
/// to [0,1]
	m_p[0] *= ONE_OVER_2PIF;
	m_p[1] *= ONE_OVER_PIF;
}

void SampleState::computeCoord(const int& nu, const int& nv)
{
	if(m_filter == Nofilter) return;
	m_wei[0] = m_p[0] * nu;
	m_wei[1] = m_p[1] * nv;
	m_u[0] = m_wei[0];
	m_wei[0] -= m_u[0];
	m_v[0] = m_wei[1];
	m_wei[1] -= m_v[0];
	
	if(m_p[0] < 0) {
		m_u[0]--;
		m_wei[0] = 1.f - m_wei[0];
	}
	if(m_p[1] < 0) {
		m_v[1]--;
		m_wei[1] = 1.f - m_wei[1];
	}
	
	if(m_filter == Nearest) {
		if(m_wei[0] >= .5f) {
			m_u[0]++;
		}
		if(m_wei[1] >= .5f) {
			m_v[0]++;
		}
	} else { /// box
		m_u[1] = m_u[0]+1;
		m_v[1] = m_v[0]+1;
	}
	
	if(m_accessU == Clamp) {
		Clamp0b(m_u[0], nu - 1);
		Clamp0b(m_u[1], nu - 1);
	} else {
		m_u[0] = m_u[0] % nu;
		m_u[1] = m_u[1] % nu;
	}
	
	if(m_accessV == Clamp) {
		Clamp0b(m_v[0], nv - 1);
		Clamp0b(m_v[1], nv - 1);
	} else {
		m_v[0] = m_v[0] % nv;
		m_v[1] = m_v[1] % nv;
	}
}

void SampleState::computeValue(float* dst, int rank)
{
	if(m_filter == Nofilter) return;
	float* a = m_buf[0];
	float* b = m_buf[1];
	float* c = m_buf[2];
	float* d = m_buf[3];
	for(int i=0;i<rank;++i) {
		a[i] += (b[i] - a[i]) * m_wei[0];
		c[i] += (d[i] - c[i]) * m_wei[0];
/// to corner0
		a[i] += (c[i] - a[i]) * m_wei[1];
	}
}

void SampleState::setValue(float* dst, int rank) const
{
/// from corner0
	const float* a = m_buf[0];
	for(int i=0;i<rank;++i) {
		dst[i] = a[i] * m_exposureWeight;
	}
/// gamma correction?
	if(m_channel == Luminance) {
		*dst = 0.2126f * a[0] + 0.7152f * a[1] + 0.0722f * a[2];
	}
}

void SampleState::setValue(float* dst) const
{
/// from corner0
	const float* a = m_buf[0];
	for(int i=0;i<m_rank;++i) {
		dst[i] = a[i] * m_exposureWeight;
	}
/// gamma correction?
	if(m_channel == Luminance) {
		*dst = 0.2126f * a[0] + 0.7152f * a[1] + 0.0722f * a[2];
	}
}

void SampleState::computeValue(float* dst)
{
	if(m_filter == Nofilter) return;
	float* a = m_buf[0];
	float* b = m_buf[1];
	float* c = m_buf[2];
	float* d = m_buf[3];
	for(int i=0;i<m_rank;++i) {
		a[i] += (b[i] - a[i]) * m_wei[0];
		c[i] += (d[i] - c[i]) * m_wei[0];
/// to corner0
		a[i] += (c[i] - a[i]) * m_wei[1];
	}
}

SampleState::ChannelMode SampleState::channel() const
{ return m_channel; }

SampleState::FilterMode SampleState::filter() const
{ return m_filter; }

SampleState::AccessMode SampleState::accessU() const
{ return m_accessU; }

SampleState::AccessMode SampleState::accessV() const
{ return m_accessV; }

}
