/*
 *  UniformField.h
 *  aloe
 *
 *  begin at origin, all cell has same cellSize
 *  grid resolution MxNxP
 *  value stored at cell corners
 *  data size (M+1)x(N+1)x(P+1)
 *
 *  Created by jian zhang on 3/2/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_UNIFORM_FIELD_H
#define ALO_UNIFORM_FIELD_H

#include <boost/scoped_array.hpp>
#include <math/miscfuncs.h>

namespace alo {

namespace sds {

template<typename T>
class UniformField {

/// (origin, cell_size, 1/cell_size)
	float m_originCellSize[5];
/// (M,N,P,M+1,N+1,P+1,(M+1)(N+1),M-1,N-1,P-1)
	int m_dim[10];
	boost::scoped_array<T> m_value;
	
public:

	UniformField();
	
/// (origin, cell_size)
	void setOriginCellSize(const float* v);
/// (M,N,P)
	void setResolution(const int* v);
/// minmax box as (x0,y0,z0,x1,y1,z1)
	void getBox(float* b) const;
/// at u
	T lookup(const float* u) const;
	const float& cellSize() const;
	
	T* value();
	const T* c_value() const;
	
protected:

private:
	int valueInd(int i, int j, int k) const;
	
};

template<typename T>
UniformField<T>::UniformField()
{}

template<typename T>
void UniformField<T>::setOriginCellSize(const float* v)
{
	memcpy(m_originCellSize, v, 16);
	m_originCellSize[4] = 1.f / m_originCellSize[3];
}

template<typename T>
void UniformField<T>::setResolution(const int* v)
{
	memcpy(m_dim, v, 12);
	m_dim[3] = m_dim[0] + 1;
	m_dim[4] = m_dim[1] + 1;
	m_dim[5] = m_dim[2] + 1;
	m_dim[6] = m_dim[3] * m_dim[4];
	m_dim[7] = m_dim[0] - 1;
	m_dim[8] = m_dim[1] - 1;
	m_dim[9] = m_dim[2] - 1;
	int dataL = DivideUp(m_dim[3] * m_dim[4] * m_dim[5], 512) * 512;
	m_value.reset(new float[dataL]);
	
}

template<typename T>
const float& UniformField<T>::cellSize() const
{ return m_originCellSize[3]; }

template<typename T>
void UniformField<T>::getBox(float* b) const
{
	memcpy(b, m_originCellSize, 12);
	b[3] = b[0] + m_originCellSize[3] * m_dim[0];
	b[4] = b[1] + m_originCellSize[3] * m_dim[1];
	b[5] = b[2] + m_originCellSize[3] * m_dim[2];
}

template<typename T>
T* UniformField<T>::value()
{ return m_value.get(); }
	
template<typename T>
const T* UniformField<T>::c_value() const
{ return m_value.get(); }

template<typename T>
int UniformField<T>::valueInd(int i, int j, int k) const
{ return k*m_dim[6]+j*m_dim[3]+i; }

template<typename T>
T UniformField<T>::lookup(const float* u) const
{
	float wx = (u[0] - m_originCellSize[0]) * m_originCellSize[4];
	if(wx<0) wx = 0;
	int i = wx;
	if(i>m_dim[7]) i = m_dim[7];
	 
	wx -= i;
	if(wx > 1.f) wx = 1.f;
	
	float wy = (u[1] - m_originCellSize[1]) * m_originCellSize[4];
	if(wy<0) wy = 0;
	int j = wy;
	if(j>m_dim[8]) j = m_dim[8];
	
	wy -= j;
	if(wy > 1.f) wy = 1.f;
	
	float wz = (u[2] - m_originCellSize[2]) * m_originCellSize[4];
	if(wz<0) wz = 0;
	int k = wz;
	if(k>m_dim[9]) k = m_dim[9];
	
	wz -= k;
	if(wz > 1.f) wz = 1.f;
	
	float a = (1.f - wx) * c_value()[valueInd(i  ,j  ,k)]
					+ wx * c_value()[valueInd(i+1,j  ,k)];
	float b = (1.f - wx) * c_value()[valueInd(i  ,j+1,k)]
					+ wx * c_value()[valueInd(i+1,j+1,k)];
	float c = (1.f - wx) * c_value()[valueInd(i  ,j  ,k+1)]
					+ wx * c_value()[valueInd(i+1,j  ,k+1)];
	float d = (1.f - wx) * c_value()[valueInd(i  ,j+1,k+1)]
					+ wx * c_value()[valueInd(i+1,j+1,k+1)];
	
	a = (1.f - wy) * a + wy * b;
	c = (1.f - wy) * c + wy * d;
	
	return (1.f - wz) * a + wz * c;
}

}

}

#endif