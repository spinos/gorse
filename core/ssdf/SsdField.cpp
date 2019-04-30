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
m_Q(0),
m_numFineValues(0)
{}

SsdField::~SsdField()
{
	destroy();
}

void SsdField::create(int p, int q, int l)
{
	m_P = p;
	m_Q = q;
	m_numFineValues = l;
	const int d = 1<<p;
	m_coarseDistance.setResolution(d);
    m_coarseNormal.setResolution(d);
	m_cellIndices.resetBuffer(d * d * d);
	m_fineDistance.resetBuffer(l);
	m_fineNormal.resetBuffer(l);
}

void SsdField::destroy()
{ 
	m_P = m_Q = m_numFineValues = 0; 
	m_coarseDistance.destroy();
	m_coarseNormal.destroy();
	m_cellIndices.purgeBuffer();
	m_fineDistance.purgeBuffer();
	m_fineNormal.purgeBuffer();
}

void SsdField::setOriginCellSize(const float* v)
{ 
    m_coarseDistance.setOriginCellSize(v);
    m_coarseNormal.setOriginCellSize(v);
    m_fieldBox.setMin(v[0], v[1], v[2]);
    const int d = m_coarseDistance.resolution();
    m_fieldBox.setMax(v[0] + v[3] * d, v[1] + v[3] * d, v[2] + v[3] * d);
}

int* SsdField::cellIndValue()
{ return m_cellIndices.data(); }

float* SsdField::coarseDistanceValue()
{ return m_coarseDistance.value(); }

float* SsdField::fineDistanceValue()
{ return m_fineDistance.data(); }

void SsdField::copyCoarseDistanceValue(const sds::CubicField<float>* b)
{ m_coarseDistance.copyValue(b); }

void SsdField::copyFineDistanceValue(const int offset, const sds::CubicField<float>* b)
{
	memcpy(&m_fineDistance.data()[offset], b->c_value(), b->numValues() * sizeof(float) );
}

void SsdField::copyCoarseNormalValue(const sds::CubicField<Vector3F>* b)
{ m_coarseNormal.copyValue(b); }

void SsdField::copyFineNormalValue(const int offset, const sds::CubicField<Vector3F>* b)
{
    memcpy(&m_fineNormal.data()[offset], b->c_value(), b->numValues() * sizeof(Vector3F) );
}

void SsdField::copyCellOffset(const int* b)
{ 
	const int d = 1<<m_P;
	memcpy(m_cellIndices.data(), b, (d*d*d)<<2 ); 
}

bool SsdField::isEmpty() const
{ return m_P < 1; }

const int& SsdField::P() const
{ return m_P; }

const int& SsdField::Q() const
{ return m_Q; }

int SsdField::numValues() const
{ return m_coarseDistance.numValues(); }

int SsdField::numCells() const
{ return m_coarseDistance.numCells(); }

const int &SsdField::numFineValues() const
{ return m_numFineValues; }

const float* SsdField::originCellSize() const
{ return m_coarseDistance.originCellSize(); }

const float* SsdField::c_coarseDistanceValue() const
{ return m_coarseDistance.c_value(); }

const float* SsdField::c_fineDistanceValue() const
{ return m_fineDistance.c_data(); }

const Vector3F *SsdField::c_coarseNormalValue() const
{ return m_coarseNormal.c_value(); }
	
const Vector3F *SsdField::c_fineNormalValue() const
{ return m_fineNormal.c_data(); }

const int* SsdField::c_cellIndValue() const
{ return m_cellIndices.c_data(); }

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

Vector3F SsdField::lookupNormal(const float* u) const
{ return m_coarseNormal.lookup(u); }

Vector3F *SsdField::coarseNormalValue()
{ return m_coarseNormal.value(); }

Vector3F *SsdField::fineNormalValue()
{ return m_fineNormal.data(); }

void SsdField::setBBox(const BoundingBox &b)
{ m_bbox = b; }

void SsdField::getAabb(float *b) const
{ memcpy(b, (const float *)&m_bbox, 24); }

void SsdField::setAabb(const float *b)
{ memcpy(&m_bbox, b, 24); }

void SsdField::expandAabb(float *b) const
{
	const float *d = m_bbox.data();
	if(b[0] > d[0]) b[0] = d[0];
	if(b[1] > d[1]) b[1] = d[1];
	if(b[2] > d[2]) b[2] = d[2];
	if(b[3] < d[3]) b[3] = d[3];
	if(b[4] < d[4]) b[4] = d[4];
	if(b[5] < d[5]) b[5] = d[5];
}

const float *SsdField::aabb() const
{ return m_bbox.data(); }

const float *SsdField::fieldAabb() const
{ return m_fieldBox.data(); }

void SsdField::verbose() const
{
	std::cout << " SsdField "<<m_P<<" "<<m_Q<<" "<<numFineValues()
	<<"\n origin ("<<originCellSize()[0]<<", "<<originCellSize()[1]<<", "<<originCellSize()[2]
	<<") cell_size "<<originCellSize()[3] << " delta " << delta()
	<<"\n aabb "<<m_bbox;
}

}

}