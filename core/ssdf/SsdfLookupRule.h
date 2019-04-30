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
#include <math/pointBox.h>
#include <math/boxBox.h>
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
    float m_boundary;
    float m_low, m_high;
	
public:

	SsdfLookupRule();
	
	void attach(const T& field);
    void detach();
    
    void setRelativeBoundaryOffset(float x);

	float lookup(const float* p) const;
	Vector3F lookupNormal(const float* p) const;
    float mapDistance(const float* p) const;
    
    const float &delta() const;
/// any nonempty cell intersects box b
/// b is (low, high)	
	bool intersectBox(const float* b) const;

	const float &boundary() const;

	void limitStepSize(float &x) const;
	
protected:

private:

	void computeCellCoord(int* u, const float* p) const;
	int computeCellInd(const int* u) const;
	int cellValueInd(int i, int j, int k) const;
	void getCellBox(float *b, int i, int j, int k) const;
/// Tv is value type
	template<typename Tv>
	Tv lookupInCell(const float *p,
					const int *u,
					const Tv *v) const;
	
};

template<typename T>
SsdfLookupRule<T>::SsdfLookupRule() : m_field(nullptr),
m_boundary(0.f)
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
    m_low = m_delta * .5f;
    m_high = m_delta * 16.f;
}

template<typename T>
void SsdfLookupRule<T>::detach()
{ m_field = nullptr; }

template<typename T>
void SsdfLookupRule<T>::setRelativeBoundaryOffset(float x)
{ m_boundary = m_delta * x; }

template<typename T>
float SsdfLookupRule<T>::lookup(const float* p) const
{
    if(!m_field) return 1e10f;
    if(m_field->isEmpty()) return 1e10f;

    float toBox = 0.f;
    float q[3];
    memcpy(q, p, 12);
    if(isPointOutsideBox(q, m_field->aabb() ) ) {
    	toBox = movePointOntoBox(q, m_field->aabb() );
    }

	int u[3];
	computeCellCoord(u, q);
	const int c = computeCellInd(u);
	const int offset = m_field->c_cellIndValue()[c];
	if(offset >-1) {
		const float* fv = &m_field->c_fineDistanceValue()[offset>>2];
		return lookupInCell<float>(q, u, fv) - m_boundary + toBox;
	}
	return m_field->lookup(q) - m_boundary + toBox; 
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
template<typename Tv>
Tv SsdfLookupRule<T>::lookupInCell(const float* p,
					const int* u,
					const Tv* v) const
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
	
	Tv a = v[cellValueInd(i  ,j  ,k)] * (1.f - wx)
			+ v[cellValueInd(i+1,j  ,k)] * wx;
	Tv b = v[cellValueInd(i  ,j+1,k)] * (1.f - wx)
			+ v[cellValueInd(i+1,j+1,k)] * wx;
	Tv c = v[cellValueInd(i  ,j  ,k+1)] * (1.f - wx)
			+ v[cellValueInd(i+1,j  ,k+1)] * wx;
	Tv d = v[cellValueInd(i  ,j+1,k+1)] * (1.f - wx)
			+ v[cellValueInd(i+1,j+1,k+1)] * wx;
	
	a = a * (1.f - wy) + b * wy;
	c = c * (1.f - wy) + d * wy;
	
	return a * (1.f - wz) + c * wz;
}

template<typename T>
const float &SsdfLookupRule<T>::delta() const
{ return m_delta; }

template<typename T>
Vector3F SsdfLookupRule<T>::lookupNormal(const float* p) const
{
	if(!m_field) return Vector3F::One;
    if(m_field->isEmpty()) Vector3F::One;
	int u[3];
	computeCellCoord(u, p);
	const int c = computeCellInd(u);
	const int offset = m_field->c_cellIndValue()[c];
	if(offset >-1) {
		const Vector3F *fv = &m_field->c_fineNormalValue()[offset>>2];
		return lookupInCell<Vector3F>(p, u, fv);
	}
	return m_field->lookupNormal(p); 
}

template<typename T>
float SsdfLookupRule<T>::mapDistance(const float* p) const
{ return lookup(p); }

template<typename T>
void SsdfLookupRule<T>::getCellBox(float *b, int i, int j, int k) const
{
	const float &h = m_originCellSize[3];
	b[0] = m_originCellSize[0] + h * i;
	b[1] = m_originCellSize[1] + h * j;
	b[2] = m_originCellSize[2] + h * k;
	b[3] = b[0] + h;
	b[4] = b[1] + h;
	b[5] = b[2] + h;
}

template<typename T>
bool SsdfLookupRule<T>::intersectBox(const float* b) const
{ 
	const int d = m_dim[0];
	float cb[6];
	int c = 0;
	for(int k=0;k<d;++k) {
		for(int j=0;j<d;++j) {
			for(int i=0;i<d;++i) {
				const int offset = m_field->c_cellIndValue()[c];
				c++;
				if(offset < 0) continue;

				getCellBox(cb, i, j, k);

				if(boxIntersectBox(cb, b)) return true;
			}
		}
	}
	
	return false; 
}

template<typename T>
const float &SsdfLookupRule<T>::boundary() const
{ return m_boundary; }

template<typename T>
void SsdfLookupRule<T>::limitStepSize(float &x) const
{ 
	if(x > 1e-3f && x < m_low) x = m_low;
	if(x > m_high) x = m_high;
}

} /// end of namespace sdf

} /// end of namespace alo

#endif
