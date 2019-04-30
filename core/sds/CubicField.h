/*
 *  CubicField.h
 *  aloe
 *
 *  uniform field has same resolution along 3 dimensions
 *  begin at origin, all cell has same cellSize
 *  grid resolution M
 *  value stored at cell corners
 *  data size (M+1)^3
 *
 *  2019/4/28
 */

#ifndef ALO_CUBIC_FIELD_H
#define ALO_CUBIC_FIELD_H

#include <math/SimpleBuffer.h>
#include <math/miscfuncs.h>
#include <iostream>
#ifdef LINUX
#include <stdlib.h>
#include <string.h>
#endif
namespace alo {

namespace sds {

template<typename T>
class CubicField {

/// (origin, cell_size, 1/cell_size)
	float m_originCellSize[5];
/// (M,M+1,(M+1)^2,M-1)
	int m_dim[4];
	SimpleBuffer<T> m_value;
	
public:

	CubicField();
	virtual ~CubicField();
	
/// (origin, cell_size)
	void setOriginCellSize(const float* v);
/// M
	void setResolution(const int& x);
/// (M+1)^3
	int numValues() const;
/// M^3
	int numCells() const;
/// box as (low, high)
	void getBox(float* b) const;
/// (center, half_span)
	void getCoord(float* c) const;
/// (origin, cell_size)
	void getOriginCellSize(float* b) const;
/// at u
	T lookup(const float* u) const;
	const float& cellSize() const;
	
	T* value();
	const T* c_value() const;
	
	const float* originCellSize() const;
	const float* origin() const;
	void setAllValues(const T& b);
/// at u to closest value
	void addValueAt(float* weight, const T& b, const float* u);
	void addValue(float* weight, const T& b, int i, int j, int k);
/// v_i <- v_i / weight_i, when weight_i > 1
	void divide(const float* weight);
	void copyValue(const CubicField<T>* b);
	
	void upSample();
/// 8 corners of cell(i,j,k)	
	void getCellValues(T* box, int i, int j, int k) const;
	
	const T& getPointValue(int i, int j, int k) const;
/// average of corner values
	T getEdgeValue(int i, int j, int k,
					int di, int dj, int dk) const;
	T getFaceValue(int i, int j, int k,
					int plane) const;
	T getCellValue(int i, int j, int k) const;
    
    void printValues() const;

    void destroy();

    const int &resolution() const;
	
protected:

private:
/// (i,j,k) is (x,y,z)
	int valueInd(int i, int j, int k) const;
	
};

template<typename T>
CubicField<T>::CubicField()
{}

template<typename T>
CubicField<T>::~CubicField()
{ destroy(); }

template<typename T>
void CubicField<T>::setOriginCellSize(const float* v)
{
	m_originCellSize[0] = v[0];
	m_originCellSize[1] = v[1];
	m_originCellSize[2] = v[2];
	m_originCellSize[3] = v[3];
	
	m_originCellSize[4] = 1.f / m_originCellSize[3];
}

template<typename T>
void CubicField<T>::setResolution(const int& x)
{
	m_dim[0] = x;
	m_dim[1] = m_dim[0] + 1;
	m_dim[2] = m_dim[1] * m_dim[1];
	m_dim[3] = m_dim[0] - 1;
	m_value.resetBuffer(m_dim[1] * m_dim[2]);
	
}

template<typename T>
const float& CubicField<T>::cellSize() const
{ return m_originCellSize[3]; }

template<typename T>
void CubicField<T>::getBox(float* b) const
{
	b[0] = m_originCellSize[0];
	b[1] = m_originCellSize[1];
	b[2] = m_originCellSize[2];
	
	b[3] = b[0] + m_originCellSize[3] * m_dim[0];
	b[4] = b[1] + m_originCellSize[3] * m_dim[0];
	b[5] = b[2] + m_originCellSize[3] * m_dim[0];
}

template<typename T>
void CubicField<T>::getCoord(float* c) const
{
	const float h = m_originCellSize[3] * (m_dim[0]>>1);
	c[0] = m_originCellSize[0] + h;
	c[1] = m_originCellSize[1] + h;
	c[2] = m_originCellSize[2] + h;
	c[3] = h;
}

template<typename T>
void CubicField<T>::getOriginCellSize(float* b) const
{ memcpy(b, m_originCellSize, 16); }

template<typename T>
T* CubicField<T>::value()
{ return m_value.data(); }
	
template<typename T>
const T* CubicField<T>::c_value() const
{ return m_value.c_data(); }

template<typename T>
int CubicField<T>::valueInd(int i, int j, int k) const
{ return k*m_dim[2]+j*m_dim[1]+i; }

template<typename T>
T CubicField<T>::lookup(const float* u) const
{
	float wx = (u[0] - m_originCellSize[0]) * m_originCellSize[4];
	if(wx<0) wx = 0;
	int i = wx;
	wx -= i;
	if(i>m_dim[3]) {
		i = m_dim[3];
		wx = 1.f;
	}
	
	float wy = (u[1] - m_originCellSize[1]) * m_originCellSize[4];
	if(wy<0) wy = 0;
	int j = wy;
	wy -= j;
	if(j>m_dim[3]) {
		j = m_dim[3];
		wy = 1.f;
	}
	
	float wz = (u[2] - m_originCellSize[2]) * m_originCellSize[4];
	if(wz<0) wz = 0;
	int k = wz;
	wz -= k;
	if(k>m_dim[3]) {
		k = m_dim[3];
		wz = 1.f;
	}
	
	T a = c_value()[valueInd(i  ,j  ,k)] * (1.f - wx)
		+ c_value()[valueInd(i+1,j  ,k)] * wx;
	T b = c_value()[valueInd(i  ,j+1,k)] * (1.f - wx)
		+ c_value()[valueInd(i+1,j+1,k)] * wx;
	T c = c_value()[valueInd(i  ,j  ,k+1)] * (1.f - wx)
		+ c_value()[valueInd(i+1,j  ,k+1)] * wx;
	T d = c_value()[valueInd(i  ,j+1,k+1)] * (1.f - wx)
		+ c_value()[valueInd(i+1,j+1,k+1)] * wx;
	
	a = a * (1.f - wy) + b * wy;
	c = c * (1.f - wy) + d * wy;
	
	return a * (1.f - wz) + c * wz;
}

template<typename T>
void CubicField<T>::copyValue(const CubicField<T>* b)
{ memcpy(value(), b->c_value(), b->numValues() * sizeof(T) ); }

template<typename T>
int CubicField<T>::numValues() const
{ return m_dim[1] * m_dim[1] * m_dim[1]; }

template<typename T>
int CubicField<T>::numCells() const
{ return m_dim[0] * m_dim[0] * m_dim[0]; }

template<typename T>
const float* CubicField<T>::originCellSize() const
{ return m_originCellSize; }

template<typename T>
const float* CubicField<T>::origin() const
{ return m_originCellSize; }

template<typename T>
void CubicField<T>::upSample()
{
	CubicField<T> old;	
	
	float orih[4];
	memcpy(orih, m_originCellSize, 16);
	old.setOriginCellSize(orih);
	const int oldD = m_dim[0];
	old.setResolution(oldD);
	old.copyValue(this);
/// half cell size
	orih[3] /= 2.f;
	setOriginCellSize(orih);
/// double resolution
	setResolution(oldD<<1);
/// point
    for(int k=0;k<=oldD;++k) {

		for(int j=0;j<=oldD;++j) {

			for(int i=0;i<=oldD;++i) {

			    m_value[valueInd(i<<1, j<<1, k<<1)] = old.getPointValue(i,j,k);
			}
		}
	}
	
/// edge
    for(int k=0;k<=oldD;++k) {

		for(int j=0;j<=oldD;++j) {

			for(int i=0;i<oldD;++i) {

			    m_value[valueInd((i<<1)+1, j<<1, k<<1)] = old.getEdgeValue(i,j,k,1,0,0);
			}
		}
	}
	
	for(int k=0;k<=oldD;++k) {

		for(int i=0;i<=oldD;++i) {
		    for(int j=0;j<oldD;++j) {
			
			    m_value[valueInd(i<<1, (j<<1)+1, k<<1)] = old.getEdgeValue(i,j,k,0,1,0);
			}
		}
	}
	
	for(int j=0;j<=oldD;++j) {

		for(int i=0;i<=oldD;++i) {
		    for(int k=0;k<oldD;++k) {

			    m_value[valueInd(i<<1, j<<1, (k<<1)+1)] = old.getEdgeValue(i,j,k,0,0,1);
			}
		}
	}

/// face
    for(int k=0;k<=oldD;++k) {

		for(int j=0;j<oldD;++j) {

			for(int i=0;i<oldD;++i) {

			    m_value[valueInd((i<<1)+1, (j<<1)+1, k<<1)] = old.getFaceValue(i,j,k,2);
			}
		}
	}
	
	for(int j=0;j<=oldD;++j) {
	    for(int k=0;k<oldD;++k) {

			for(int i=0;i<oldD;++i) {

			    m_value[valueInd((i<<1)+1, j<<1, (k<<1)+1)] = old.getFaceValue(i,j,k,1);
			}
		}
	}
	
	for(int i=0;i<=oldD;++i) {

	    for(int j=0;j< oldD;++j) {
	        for(int k=0;k<oldD;++k) {

			    m_value[valueInd(i<<1, (j<<1)+1, (k<<1)+1)] = old.getFaceValue(i,j,k,0);
			}
		}
	}
/// volume
	for(int k=0;k<oldD;++k) {

		for(int j=0;j<oldD;++j) {

			for(int i=0;i<oldD;++i) {
				m_value[valueInd((i<<1)+1, (j<<1)+1, (k<<1)+1)] = old.getCellValue(i,j,k);
			}
		}
	}
	
}

template<typename T>
void CubicField<T>::getCellValues(T* box, int i, int j, int k) const
{
    box[0] = c_value()[valueInd(i,j,k)];
    box[1] = c_value()[valueInd(i+1,j,k)];
    box[2] = c_value()[valueInd(i,j+1,k)];
    box[3] = c_value()[valueInd(i+1,j+1,k)];
    box[4] = c_value()[valueInd(i,j,k+1)];
    box[5] = c_value()[valueInd(i+1,j,k+1)];
    box[6] = c_value()[valueInd(i,j+1,k+1)];
    box[7] = c_value()[valueInd(i+1,j+1,k+1)];
            
}

template<typename T>
const T& CubicField<T>::getPointValue(int i, int j, int k) const
{ return c_value()[valueInd(i,j,k)]; }

template<typename T>
T CubicField<T>::getEdgeValue(int i, int j, int k,
					int di, int dj, int dk) const
{
	return (c_value()[valueInd(i,j,k)]
			+ c_value()[valueInd(i+di,j+dj,k+dk)]) * .5f;
}

template<typename T>
T CubicField<T>::getFaceValue(int i, int j, int k, int plane) const
{
	if(plane==2)
		return (c_value()[valueInd(i,j,k)]
			+ c_value()[valueInd(i+1,j,k)]
			+ c_value()[valueInd(i,j+1,k)]
			+ c_value()[valueInd(i+1,j+1,k)]) * .25f;
	if(plane==1)
		return (c_value()[valueInd(i,j,k)]
			+ c_value()[valueInd(i+1,j,k)]
			+ c_value()[valueInd(i,j,k+1)]
			+ c_value()[valueInd(i+1,j,k+1)]) * .25f;
	return (c_value()[valueInd(i,j,k)]
			+ c_value()[valueInd(i,j+1,k)]
			+ c_value()[valueInd(i,j,k+1)]
			+ c_value()[valueInd(i,j+1,k+1)]) * .25f;
}

template<typename T>
T CubicField<T>::getCellValue(int i, int j, int k) const
{
	return (c_value()[valueInd(i,j,k)]
		+ c_value()[valueInd(i+1,j,k)]
		+ c_value()[valueInd(i,j+1,k)]
		+ c_value()[valueInd(i+1,j+1,k)]
		+ c_value()[valueInd(i,j,k+1)]
		+ c_value()[valueInd(i+1,j,k+1)]
		+ c_value()[valueInd(i,j+1,k+1)]
		+ c_value()[valueInd(i+1,j+1,k+1)]) * .125f;
}

template<typename T>
void CubicField<T>::setAllValues(const T& b)
{
	const int n = numValues();
	for(int i=0;i<n;++i)
		m_value[i] = b;
}

template<typename T>
void CubicField<T>::addValueAt(float* weight, const T& b, const float* u)
{
	float wx = (u[0] - m_originCellSize[0]) * m_originCellSize[4];
	int i = wx;
	if(i<0 || i>=m_dim[0]) return;
	
	if(i>m_dim[3]) i = m_dim[3];
	
	float wy = (u[1] - m_originCellSize[1]) * m_originCellSize[4];
	int j = wy;
	if(j<0 || j>=m_dim[0]) return;
	
	if(j>m_dim[3]) j = m_dim[3];
	
	float wz = (u[2] - m_originCellSize[2]) * m_originCellSize[4];
	int k = wz;
	if(k<0 || k>=m_dim[0]) return;
	
	if(wx - i > .5f) 
		i++;
		
	if(wy - j > .5f) 
		j++;
		
	if(wz - k > .5f) 
		k++;

	addValue(weight, b, i,j,k);
}

template<typename T>
void CubicField<T>::addValue(float* weight, const T& b, int i, int j, int k)
{
	int c = valueInd(i,j,k);
	if(weight[c] < .1f) {
		m_value[c] = b;
		weight[c] = 1.f;
		return;
	} 
	
	m_value[c] += b;
	weight[c] += 1.f;
}

template<typename T>
void CubicField<T>::divide(const float* weight)
{
	const int n = numValues();
	for(int i=0;i<n;++i) {
		if(weight[i] > 1.f) {
			m_value[i] /= weight[i];
		}
	}
}

template<typename T>
void CubicField<T>::printValues() const
{
    const int n = numValues();
	for(int i=0;i<n;++i) {
        std::cout << m_value[i];
	}
}

template<typename T>
void CubicField<T>::destroy()
{ m_value.purgeBuffer(); }

template<typename T>
const int &CubicField<T>::resolution() const
{ return m_dim[0]; }

}

}

#endif
