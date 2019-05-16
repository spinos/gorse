/*
 *  SvField.cpp
 *  aloe
 *
 *  Created by jian zhang on 3/4/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#include "SvField.h"
#include <math/miscfuncs.h>

namespace alo {

namespace svf {

SvField::SvField() :
m_P(0), 
m_Q(0)
{}

SvField::~SvField()
{}

void SvField::create(int p, int q, int lpDistanceDataSize)
{
	m_P = p;
	m_Q = q;
	m_dim[0] = 1<<m_P;
	m_dim[1] = 1<<(m_Q - m_P);
	m_dim[2] = m_dim[0] - 1;
	m_dim[3] = m_dim[1] - 1;
	m_lpSize = lpDistanceDataSize;
	m_cellIndices.reset(new int[cellIndLength()]);
	m_lpValue.reset(new char[lpDistanceDataSize]);
	memset(m_lpValue.get(), 0, lpDistanceDataSize);
}

void SvField::setOriginCellSize(const float* v)
{ 
	memcpy(m_coord, v, 16);
	m_coord[4] = 1.f / m_coord[3];
	m_coord[5] = 1.f / m_coord[3] * m_dim[1];
}

int* SvField::cellIndValue()
{ return m_cellIndices.get(); }

char* SvField::value()
{ return m_lpValue.get(); }

void SvField::copyCellOffset(const int* b)
{ 
	const int d = 1<<m_P;
	memcpy(m_cellIndices.get(), b, (d*d*d)<<2 ); 
}

void SvField::copyCellValue(const int offset, const char* b, const int dataSize)
{
	memcpy(&value()[offset], b, dataSize);
}

const float* SvField::originCellSize() const
{ return m_coord; }

const int& SvField::P() const
{ return m_P; }

const int& SvField::Q() const
{ return m_Q; }

const int& SvField::valueStorageSize() const
{ return m_lpSize; }

const char* SvField::c_value() const
{ return m_lpValue.get(); }

int SvField::cellIndLength() const
{ 
	const int d = 1<<m_P;
	return DivideUp(d*d*d, 256)<<8; 
}

const int* SvField::c_cellIndValue() const
{ return m_cellIndices.get(); }

void SvField::getOriginCellSize(float* b) const
{ memcpy(b, m_coord, 16); }

const float& SvField::cellSize() const
{ return m_coord[3]; }

void SvField::getBox(float* b) const
{
	memcpy(b, m_coord, 12);
	b[3] = b[4] = b[5] = m_coord[3] * m_dim[0];
	b[3] += b[0];
	b[4] += b[1];
	b[5] += b[2];
}

void SvField::getCellBox(float* b, int i, int j, int k) const
{
	b[0] = m_coord[0] + m_coord[3] * i;
	b[1] = m_coord[1] + m_coord[3] * j;
	b[2] = m_coord[2] + m_coord[3] * k;
	b[3] = b[0] + m_coord[3];
	b[4] = b[1] + m_coord[3];
	b[5] = b[2] + m_coord[3];
}

void SvField::getCoord(float* c) const
{
	c[3] = m_coord[3] * (1<<(m_P-1));
	c[0] = m_coord[0] + c[3];
	c[1] = m_coord[1] + c[3];
	c[2] = m_coord[2] + c[3];
}

float SvField::delta() const
{ return cellSize() / (1<<(m_Q - m_P)); }

const int& SvField::numCellsPerDimension() const
{ return m_dim[0]; }

const int& SvField::numCellDivisionPerDimension() const
{ return m_dim[1]; }

bool SvField::isCellEmpty(int i, int j, int k) const
{ return c_cellIndValue()[k*m_dim[0]*m_dim[0] + j*m_dim[0] + i] < 0; }

void SvField::computeCellCoord(int* u, const float* p) const
{
	u[0] = (p[0] - m_coord[0]) * m_coord[4];
	u[1] = (p[1] - m_coord[1]) * m_coord[4];
	u[2] = (p[2] - m_coord[2]) * m_coord[4];
	Clamp0b(u[0], m_dim[0]);
	Clamp0b(u[1], m_dim[0]);
	Clamp0b(u[2], m_dim[0]);
}

int SvField::computeCellInd(const int* u) const
{ return (u[2]*m_dim[0]*m_dim[0] + u[1]*m_dim[0] + u[0]); }

int SvField::cellValueInd(int i, int j, int k) const
{ return k*(m_dim[1]+1)*(m_dim[1]+1) + j*(m_dim[1]+1) + i; }

void SvField::computeCellWeight(float& wx, float& wy, float& wz,
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

void SvField::getCellValue(char* b, int size, int i, int j, int k) const
{
	int offset = c_cellIndValue()[k*m_dim[0]*m_dim[0] + j*m_dim[0] + i];
	const char* a = &c_value()[offset];
	//std::cout<<" offset "<<offset<<" "<<i<<","<<j<<","<<k
	//	<<" size "<<size;
	//std::cout.flush();
	memcpy(b, a, size); 
}

void SvField::computeLookupCoord(LookupCoord& dst, const float* p) const
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

void SvField::computeLookupCoordU(LookupCoord& dst, const float* p, const int* u) const
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

void SvField::searchNonEmptyCell(LookupCoord& dst, const float* p) const
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

}

}