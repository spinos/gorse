/*
 *  SsdfContext.h
 *  aloe
 *
 *  ssdf ray march
 *  T is ssdf type
 *  Tr is lookup rule
 *
 *  Created by jian zhang on 3/7/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SSDF_CONTEXT_H
#define ALO_SSDF_CONTEXT_H

#include <math/Ray.h>

namespace alo {

namespace sdf {

template<typename T, typename Tr>
class SsdfContext {

	Tr m_rule;
/// (origin, direction, t_min, t_max)
	float m_ray[8];
	float m_tmin, m_tmax;
	float m_pnt[3];
	float m_nml[3];

public:

	SsdfContext();
	
	void setRay(const float* ray);
	void setRay(const Ray& ray);

	bool intersect(const T& field);
	
	template<typename Ts>
	void getResult(Ts& result) const;
	
protected:

private:

};

template<typename T, typename Tr>
SsdfContext<T, Tr>::SsdfContext()
{}

template<typename T, typename Tr>
void SsdfContext<T, Tr>::setRay(const float* ray)
{ memcpy(m_ray, ray, 32); }

template<typename T, typename Tr>
void SsdfContext<T, Tr>::setRay(const Ray& ray)
{
	memcpy(m_ray, &ray, 12);
	memcpy(&m_ray[3], &ray.m_dir, 12);
	m_ray[6] = ray.m_tmin;
	m_ray[7] = ray.m_tmax;
}

template<typename T, typename Tr>
bool SsdfContext<T, Tr>::intersect(const T& field)
{
	m_rule.attach(field);
	
	if(!m_rule.intersectBox(m_ray) )
		return false;
		
	m_tmin = m_ray[6];
	m_tmax = m_ray[7];
	m_pnt[0] = m_ray[0] + m_ray[3] * m_tmin;
	m_pnt[1] = m_ray[1] + m_ray[4] * m_tmin;
	m_pnt[2] = m_ray[2] + m_ray[5] * m_tmin;
	
	const float minStep = field.delta() * 1.4f;
	const float thre = minStep * .7f;
	
	float dist = m_rule.lookup(m_pnt);
	
	int step = 0;
	while(dist > thre) {
		if(dist < minStep)
			dist = minStep;
			
		m_tmin += dist;
		if(m_tmin > m_tmax)
			break;
			//return false;
			
		m_pnt[0] += m_ray[3] * dist;
		m_pnt[1] += m_ray[4] * dist;
		m_pnt[2] += m_ray[5] * dist;
		
		dist = m_rule.lookup(m_pnt);

		step++;
		if(step > 39)
			break;
	}

	float fstep = (float)step / 40.f;
	m_nml[0] = fstep;
	m_nml[1] = fstep;
	m_nml[2] = fstep;
	return true;
}

template<typename T, typename Tr>
template<typename Ts>
void SsdfContext<T, Tr>::getResult(Ts& result) const
{
	memcpy(&result.hitNormal(), m_nml, 12);
}

}

}

#endif

