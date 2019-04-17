/*
 *  SsdField.cpp
 *  aloe
 *
 *  Created by jian zhang on 3/4/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#include "SsdField.h"
#include <math/miscfuncs.h>

namespace alo {

namespace sdf {

SsdField::SsdField() :
m_P(0), 
m_Q(0)
{}

SsdField::~SsdField()
{}

void SsdField::create(int p, int q, int lpDistanceDataSize)
{
	m_P = p;
	m_Q = q;
	m_lpSize = lpDistanceDataSize;
	const int d = 1<<p;
	m_coarseDistance.setResolution(d);
	m_cellIndices.reset(new int[cellIndLength()]);
	m_lpDistance.reset(new float[lpDistanceDataSize>>2]);
	
}

void SsdField::setOriginCellSize(const float* v)
{ m_coarseDistance.setOriginCellSize(v); }

int* SsdField::cellIndValue()
{ return m_cellIndices.get(); }

float* SsdField::coarseDistanceValue()
{ return m_coarseDistance.value(); }

float* SsdField::fineDistanceValue()
{ return m_lpDistance.get(); }

void SsdField::copyCoarseDistanceValue(const sds::CubicField<float>* b)
{ m_coarseDistance.copyValue(b); }

void SsdField::copyFineDistanceValue(const int offset, const sds::CubicField<float>* b)
{
	char* dst = (char*)m_lpDistance.get();
	memcpy(&dst[offset], b->c_value(), b->storageSize() );
}

void SsdField::copyCellOffset(const int* b)
{ 
	const int d = 1<<m_P;
	memcpy(m_cellIndices.get(), b, (d*d*d)<<2 ); 
}

const float* SsdField::originCellSize() const
{ return m_coarseDistance.originCellSize(); }

const int& SsdField::P() const
{ return m_P; }

const int& SsdField::Q() const
{ return m_Q; }

int SsdField::coarsDistanceStorageSize() const
{ return m_coarseDistance.storageSize(); }

const float* SsdField::c_coarseDistanceValue() const
{ return m_coarseDistance.c_value(); }

const int& SsdField::fineDistanceStorageSize() const
{ return m_lpSize; }

const float* SsdField::c_fineDistanceValue() const
{ return m_lpDistance.get(); }

int SsdField::cellIndLength() const
{ 
	const int d = 1<<m_P;
	return DivideUp(d*d*d, 256)<<8; 
}

const int* SsdField::c_cellIndValue() const
{ return m_cellIndices.get(); }

void SsdField::getBox(float* b) const
{ m_coarseDistance.getBox(b); }

void SsdField::getCoord(float* c) const
{ m_coarseDistance.getCoord(c); }

void SsdField::getOriginCellSize(float* b) const
{ m_coarseDistance.getOriginCellSize(b); }

const float& SsdField::cellSize() const
{ return m_coarseDistance.cellSize(); }

float SsdField::delta() const
{ return cellSize() / (1<<(m_Q - m_P)); }

float SsdField::lookup(const float* u) const
{ return m_coarseDistance.lookup(u); }

}

}