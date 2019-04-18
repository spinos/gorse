/*
 *  SsdfLookupRule.h
 *  aloe
 *
 *  ssdf lookup
 *  T is ssdf type
 *
 *  Created by jian zhang on 3/7/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SSDF_LOOKUP_RULE_H
#define ALO_SSDF_LOOKUP_RULE_H

#include <math/rayBox.h>
#include <math/miscfuncs.h>

namespace alo {

namespace sdf {

template<typename T>
class SsdfLookupRule {

/// (center, half_span)
	float m_coord[4];
	float m_originCellSize[4];
	float m_invCellSize[2];
	int m_dim[4];
	const T* m_field;
    float m_delta;
	
public:

	SsdfLookupRule();
	
	void attach(const T& field);
    void dettach();
/// ray is (origin, direction, t0, t1)	
	bool intersectBox(float* ray) const;
	
	float lookup(const float* p) const;
    
    const float &delta() const;
	
protected:

private:

	void computeCellCoord(int* u, const float* p) const;
	int computeCellInd(const int* u) const;
	int cellValueInd(int i, int j, int k) const;
	float lookupInCell(const float* p,
					const int* u,
					const float* v) const;
	
};

template<typename T>
SsdfLookupRule<T>::SsdfLookupRule() : m_field(nullptr)
{}

template<typename T>
void SsdfLookupRule<T>::attach(const T& field)
{ 
	field.getCoord(m_coord); 
	field.getOriginCellSize(m_originCellSize);
	m_invCellSize[0] = 1.f/ m_originCellSize[3];
	const int d = 1<<(field.Q() - field.P());
	m_invCellSize[1] = m_invCellSize[0] * d;
	m_dim[0] = 1<<field.P();
	m_dim[1] = d;
	m_dim[2] = m_dim[0] - 1;
	m_dim[3] = m_dim[1] - 1;
	m_field = &field;
    m_delta = field.delta();
}

template<typename T>
void SsdfLookupRule<T>::dettach()
{ m_field = nullptr; }

template<typename T>
bool SsdfLookupRule<T>::intersectBox(float* ray) const
{ return rayBoxIntersect(ray, m_coord); }

template<typename T>
float SsdfLookupRule<T>::lookup(const float* p) const
{
    if(!m_field) return 1e10f;
    if(m_field->isEmpty()) return 1e10f;
	int u[3];
	computeCellCoord(u, p);
	const int c = computeCellInd(u);
	const int offset = m_field->c_cellIndValue()[c];
	if(offset >-1) {
		const float* fv = &m_field->c_fineDistanceValue()[offset>>2];
		return lookupInCell(p, u, fv);
	}
	return m_field->lookup(p); 
}

template<typename T>
void SsdfLookupRule<T>::computeCellCoord(int* u, const float* p) const
{
	u[0] = (p[0] - m_originCellSize[0]) * m_invCellSize[0];
	u[1] = (p[1] - m_originCellSize[1]) * m_invCellSize[0];
	u[2] = (p[2] - m_originCellSize[2]) * m_invCellSize[0];
	Clamp0b(u[0], m_dim[2]);
	Clamp0b(u[1], m_dim[2]);
	Clamp0b(u[2], m_dim[2]);
}

template<typename T>
int SsdfLookupRule<T>::computeCellInd(const int* u) const
{ return (u[2]*m_dim[0]*m_dim[0] + u[1]*m_dim[0] + u[0]); }

template<typename T>
int SsdfLookupRule<T>::cellValueInd(int i, int j, int k) const
{ return k*(m_dim[1]+1)*(m_dim[1]+1) + j*(m_dim[1]+1) + i; }

template<typename T>
float SsdfLookupRule<T>::lookupInCell(const float* p,
					const int* u,
					const float* v) const
{
	float wx = (p[0] - m_originCellSize[0] - u[0] * m_originCellSize[3]) * m_invCellSize[1];
	if(wx<0) wx = 0;
	int i = wx;
	wx = wx - i;
	if(i>m_dim[3]) {
		i = m_dim[3];
		wx = 1.f;
	}
	 
	float wy = (p[1] - m_originCellSize[1] - u[1] * m_originCellSize[3]) * m_invCellSize[1];
	if(wy<0) wy = 0;
	int j = wy;
	wy = wy - j;
	if(j>m_dim[3]) {
		j = m_dim[3];
		wy = 1.f;
	}
	
	float wz = (p[2] - m_originCellSize[2] - u[2] * m_originCellSize[3]) * m_invCellSize[1];
	if(wz<0) wz = 0;
	int k = wz;
	wz = wz - k;
	if(k>m_dim[3]) {
		k = m_dim[3];
		wz = 1.f;
	}
	
	float a = v[cellValueInd(i  ,j  ,k)] * (1.f - wx)
			+ v[cellValueInd(i+1,j  ,k)] * wx;
	float b = v[cellValueInd(i  ,j+1,k)] * (1.f - wx)
			+ v[cellValueInd(i+1,j+1,k)] * wx;
	float c = v[cellValueInd(i  ,j  ,k+1)] * (1.f - wx)
			+ v[cellValueInd(i+1,j  ,k+1)] * wx;
	float d = v[cellValueInd(i  ,j+1,k+1)] * (1.f - wx)
			+ v[cellValueInd(i+1,j+1,k+1)] * wx;
	
	a = a * (1.f - wy) + b * wy;
	c = c * (1.f - wy) + d * wy;
	
	return a * (1.f - wz) + c * wz;
}

template<typename T>
const float &SsdfLookupRule<T>::delta() const
{ return m_delta; }

}

}

#endif

