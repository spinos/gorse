/*
 *  SvField.h
 *  aloe
 *
 *  sparse voxel field of unknown P, Q, data_type
 *  
 *  Created by jian zhang on 3/4/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SV_FIELD_H
#define ALO_SV_FIELD_H

#include <sds/CubicField.h>
#include <boost/scoped_array.hpp>

namespace aphid {

namespace svf {

template<typename T>
class SvField {

	SimpleBuffer<T> m_qValues;
/// to (1<<P)^3 level p cells
	boost::scoped_array<int> m_cellIndices;
/// (origin, cell_size, 1/cell_size, 1/q_cell_size)
	float m_coord[6];
/// (1<<P, 1<<(Q-P), (1<<P)-1, (1<<(Q-P))-1)
	int m_dim[4];
	int m_P, m_Q, m_numQValue;
	
public:

	struct LookupCoord {
/// trilinear
		int _ind[8];
        float _wei[3];
/// to data
        int _offset;
    };

	SvField();
	~SvField();
	
	void create(int p, int q, int numQValue);
/// (origin, cell_size)
	void setOriginCellSize(const float* v);
	
	const int& P() const;
	const int& Q() const;
	const int &numQValues() const;
	const float* originCellSize() const;
	int cellIndLength() const;
	int valueStorageSize() const;
	const T* c_value() const;
	const int* c_cellIndValue() const;
	
	int* cellIndValue();
	T* value();
	
	void copyCellOffset(const int* b);
	void copyCellValue(const int offset, const char* b, const int dataSize);
	
	void getCellValue(char* b, int size, int i, int j, int k) const;
/// minmax box as (x0,y0,z0,x1,y1,z1)
	void getBox(float* b) const;
	void getCellBox(float* b, int i, int j, int k) const;
/// (center, half_span)
	void getCoord(float* c) const;
	void getOriginCellSize(float* b) const;
	const float& cellSize() const;
	float delta() const;
	const int& numCellsPerDimension() const;
	const int& numCellDivisionPerDimension() const;
	bool isCellEmpty(int i, int j, int k) const;
	
/// at p
	T lookup(const float* p) const;
	
/// via coord
	T lookup(const LookupCoord& c) const;
	
	void computeLookupCoord(LookupCoord& dst, const float* p ) const;
	
protected:
	
	void computeCellCoord(int* u, const float* p) const;
	int computeCellInd(const int* u) const;
	
	void lookupInCell(T& dst, const float* p, const int* u,
					const T* v) const;
	
private:

	void computeCellWeight(float& wx, float& wy, float& wz,
						int& i, int& j, int& k,
						const float* p, const int* u) const;
	int cellValueInd(int i, int j, int k) const;
	
/// around p
 	void searchNonEmptyCell(LookupCoord& dst, const float* p) const;
/// in cell u
 	void computeLookupCoordU(LookupCoord& dst, const float* p, const int* u) const;

};

template<typename T>
SvField<T>::SvField() :
m_P(0), 
m_Q(0),
m_numQValue(0)
{}

template<typename T>
SvField<T>::~SvField()
{}

template<typename T>
void SvField<T>::create(int p, int q, int numQValue)
{
	m_P = p;
	m_Q = q;
	m_dim[0] = 1<<m_P;
	m_dim[1] = 1<<(m_Q - m_P);
	m_dim[2] = m_dim[0] - 1;
	m_dim[3] = m_dim[1] - 1;
	m_numQValue = numQValue;
	m_cellIndices.reset(new int[cellIndLength()]);
	m_qValues.resetBuffer(numQValue);
	memset(m_qValues.data(), 0, numQValue * sizeof(T));
}

template<typename T>
void SvField<T>::setOriginCellSize(const float* v)
{ 
	memcpy(m_coord, v, 16);
	m_coord[4] = 1.f / m_coord[3];
	m_coord[5] = 1.f / m_coord[3] * m_dim[1];
}

template<typename T>
int* SvField<T>::cellIndValue()
{ return m_cellIndices.get(); }

template<typename T>
T* SvField<T>::value()
{ return m_qValues.data(); }

template<typename T>
void SvField<T>::copyCellOffset(const int* b)
{ 
	const int d = 1<<m_P;
	memcpy(m_cellIndices.get(), b, (d*d*d)<<2 ); 
}

template<typename T>
void SvField<T>::copyCellValue(const int offset, const char* b, const int dataSize)
{
	memcpy(&value()[offset], b, dataSize);
}

template<typename T>
const float* SvField<T>::originCellSize() const
{ return m_coord; }

template<typename T>
const int& SvField<T>::P() const
{ return m_P; }

template<typename T>
const int& SvField<T>::Q() const
{ return m_Q; }

template<typename T>
const int &SvField<T>::numQValues() const
{ return m_numQValue; }

template<typename T>
int SvField<T>::valueStorageSize() const
{ return m_qValues.capacityByteSize(); }

template<typename T>
const T* SvField<T>::c_value() const
{ return m_qValues.c_data(); }

template<typename T>
int SvField<T>::cellIndLength() const
{ 
	const int d = 1<<m_P;
	return DivideUp(d*d*d, 256)<<8; 
}

template<typename T>
const int* SvField<T>::c_cellIndValue() const
{ return m_cellIndices.get(); }

template<typename T>
void SvField<T>::getOriginCellSize(float* b) const
{ memcpy(b, m_coord, 16); }

template<typename T>
const float& SvField<T>::cellSize() const
{ return m_coord[3]; }

template<typename T>
void SvField<T>::getBox(float* b) const
{
	memcpy(b, m_coord, 12);
	b[3] = b[4] = b[5] = m_coord[3] * m_dim[0];
	b[3] += b[0];
	b[4] += b[1];
	b[5] += b[2];
}

template<typename T>
void SvField<T>::getCellBox(float* b, int i, int j, int k) const
{
	b[0] = m_coord[0] + m_coord[3] * i;
	b[1] = m_coord[1] + m_coord[3] * j;
	b[2] = m_coord[2] + m_coord[3] * k;
	b[3] = b[0] + m_coord[3];
	b[4] = b[1] + m_coord[3];
	b[5] = b[2] + m_coord[3];
}

template<typename T>
void SvField<T>::getCoord(float* c) const
{
	c[3] = m_coord[3] * (1<<(m_P-1));
	c[0] = m_coord[0] + c[3];
	c[1] = m_coord[1] + c[3];
	c[2] = m_coord[2] + c[3];
}

template<typename T>
float SvField<T>::delta() const
{ return cellSize() / (1<<(m_Q - m_P)); }

template<typename T>
const int& SvField<T>::numCellsPerDimension() const
{ return m_dim[0]; }

template<typename T>
const int& SvField<T>::numCellDivisionPerDimension() const
{ return m_dim[1]; }

template<typename T>
bool SvField<T>::isCellEmpty(int i, int j, int k) const
{ return c_cellIndValue()[k*m_dim[0]*m_dim[0] + j*m_dim[0] + i] < 0; }

template<typename T>
void SvField<T>::computeCellCoord(int* u, const float* p) const
{
	u[0] = (p[0] - m_coord[0]) * m_coord[4];
	u[1] = (p[1] - m_coord[1]) * m_coord[4];
	u[2] = (p[2] - m_coord[2]) * m_coord[4];
	Clamp0b(u[0], m_dim[0]);
	Clamp0b(u[1], m_dim[0]);
	Clamp0b(u[2], m_dim[0]);
}

template<typename T>
int SvField<T>::computeCellInd(const int* u) const
{ return (u[2]*m_dim[0]*m_dim[0] + u[1]*m_dim[0] + u[0]); }

template<typename T>
int SvField<T>::cellValueInd(int i, int j, int k) const
{ return k*(m_dim[1]+1)*(m_dim[1]+1) + j*(m_dim[1]+1) + i; }

template<typename T>
void SvField<T>::computeCellWeight(float& wx, float& wy, float& wz,
						int& i, int& j, int& k,
						const float* p, const int* u) const
{
	wx = (p[0] - m_coord[0] - u[0] * m_coord[3]) * m_coord[5];
	if(wx<0) wx = 0;
	i = wx;
	wx = wx - i;
	if(i>m_dim[3]) {
		i = m_dim[3];
		wx = 1.f;
	}
	 
	wy = (p[1] - m_coord[1] - u[1] * m_coord[3]) * m_coord[5];
	if(wy<0) wy = 0;
	j = wy;
	wy = wy - j;
	if(j>m_dim[3]) {
		j = m_dim[3];
		wy = 1.f;
	}
	
	wz = (p[2] - m_coord[2] - u[2] * m_coord[3]) * m_coord[5];
	if(wz<0) wz = 0;
	k = wz;
	wz = wz - k;
	if(k>m_dim[3]) {
		k = m_dim[3];
		wz = 1.f;
	}
	
}

template<typename T>
void SvField<T>::getCellValue(char* b, int size, int i, int j, int k) const
{
	int offset = c_cellIndValue()[k*m_dim[0]*m_dim[0] + j*m_dim[0] + i];
	const char* a = &c_value()[offset];
	//std::cout<<" offset "<<offset<<" "<<i<<","<<j<<","<<k
	//	<<" size "<<size;
	//std::cout.flush();
	memcpy(b, a, size); 
}

template<typename T>
void SvField<T>::computeLookupCoord(LookupCoord& dst, const float* p) const
{
    int u[3];
	computeCellCoord(u, p);
	const int c = computeCellInd(u);
	const int offset = c_cellIndValue()[c];
	if(offset > -1) {
		computeLookupCoordU(dst, p, u);
		dst._offset = offset;
	} else
		searchNonEmptyCell(dst, p);
}

template<typename T>
void SvField<T>::computeLookupCoordU(LookupCoord& dst, const float* p, const int* u) const
{
    float wx, wy, wz;
	int i, j, k;
	computeCellWeight(wx, wy, wz, i, j, k, p, u);
    dst._ind[0] = cellValueInd(i  ,j  ,k);
    dst._ind[1] = cellValueInd(i+1,j  ,k);
    dst._ind[2] = cellValueInd(i  ,j+1,k);
    dst._ind[3] = cellValueInd(i+1,j+1,k);
    dst._ind[4] = cellValueInd(i  ,j  ,k+1);
    dst._ind[5] = cellValueInd(i+1,j  ,k+1);
    dst._ind[6] = cellValueInd(i  ,j+1,k+1);
    dst._ind[7] = cellValueInd(i+1,j+1,k+1);
    dst._wei[0] = wx;
    dst._wei[1] = wy;
    dst._wei[2] = wz;
}

template<typename T>
void SvField<T>::searchNonEmptyCell(LookupCoord& dst, const float* p) const
{
	float q[3];
	int u[3];
	float minD = 1e8f;
    const float delta = m_coord[3] * .5f;
    for(int i=0; i<512;++i) {
        q[0] = p[0] + RandomFn11() * delta;
        q[1] = p[1] + RandomFn11() * delta;
        q[2] = p[2] + RandomFn11() * delta;
        
        computeCellCoord(u, q);
        const int c = computeCellInd(u);
        const int offset = c_cellIndValue()[c];
        if(offset < 0) continue;
        
        float d = (q[0] - p[0]) * (q[0] - p[0])
                + (q[1] - p[1]) * (q[1] - p[1])
                + (q[2] - p[2]) * (q[2] - p[2]);
        if(minD > d) {
            minD = d;
        } else
            continue;
        
        computeLookupCoordU(dst, q, u);
		dst._offset = offset;
    }
/// not found any
    if(minD > 1e7f) dst._offset = -1;
}

template<typename T>
T SvField<T>::lookup(const float* p) const
{
	T res;
	int u[3];
	computeCellCoord(u, p);
	const int c = computeCellInd(u);
	const int offset = c_cellIndValue()[c];
	if(offset > -1) {
		const T* fv = &c_value()[offset];
		lookupInCell(res, p, u, fv);
	} else {
		std::cout<<"\n empty cell"<<u[0]<<","<<u[1]<<","<<u[2];
	}
	return res;
}

template<typename T>
void SvField<T>::lookupInCell(T& dst, const float* p, const int* u,
					const T* v) const
{
	float wx, wy, wz;
	int i, j, k;
	computeCellWeight(wx, wy, wz, i, j, k, p, u);
	
	T a = v[cellValueInd(i  ,j  ,k)] * (1.f - wx)
			+ v[cellValueInd(i+1,j  ,k)] * wx;
	T b = v[cellValueInd(i  ,j+1,k)] * (1.f - wx)
			+ v[cellValueInd(i+1,j+1,k)] * wx;
	T c = v[cellValueInd(i  ,j  ,k+1)] * (1.f - wx)
			+ v[cellValueInd(i+1,j  ,k+1)] * wx;
	T d = v[cellValueInd(i  ,j+1,k+1)] * (1.f - wx)
			+ v[cellValueInd(i+1,j+1,k+1)] * wx;
	
	a = a * (1.f - wy) + b * wy;
	c = c * (1.f - wy) + d * wy;
	
	dst = a * (1.f - wz) + c * wz;
}

template<typename T>
T SvField<T>::lookup(const LookupCoord& coord) const
{
    T res;
    if(coord._offset < 0)
        return res;
    
    const T* fv = (const T*)&c_value()[coord._offset];
    
    T a = fv[coord._ind[0]] * (1.f - coord._wei[0])
		+ fv[coord._ind[1]] * coord._wei[0];
	T b = fv[coord._ind[2]] * (1.f - coord._wei[0])
		+ fv[coord._ind[3]] * coord._wei[0];
	T c = fv[coord._ind[4]] * (1.f - coord._wei[0])
		+ fv[coord._ind[5]] * coord._wei[0];
	T d = fv[coord._ind[6]] * (1.f - coord._wei[0])
		+ fv[coord._ind[7]] * coord._wei[0];
	
	a = a * (1.f - coord._wei[1]) + b * coord._wei[1];
	c = c * (1.f - coord._wei[1]) + d * coord._wei[1];
	
	res = a * (1.f - coord._wei[2]) + c * coord._wei[2];
	return res;
}

}

}

#endif
