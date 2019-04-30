/*
 *  SsdField.cpp
 *  aloe
 *
 *  2019/4/30
 */

#include "SsdField.h"
#include <math/boxBox.h>

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
	PGridTyp::create(d);
    m_coarseNormal.create(d);
	m_fineDistance.resetBuffer(l);
	m_fineNormal.resetBuffer(l);
}

void SsdField::destroy()
{ 
	m_P = m_Q = m_numFineValues = 0; 
	PGridTyp::destroy();
	m_coarseNormal.destroy();
	m_fineDistance.purgeBuffer();
	m_fineNormal.purgeBuffer();
}

void SsdField::setOriginCellSize(const float* v)
{ 
    PGridTyp::setOriginCellSize(v);
    m_coarseNormal.setOriginCellSize(v);
}

float* SsdField::fineDistanceValue()
{ return m_fineDistance.data(); }

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

bool SsdField::isEmpty() const
{ return m_P < 1; }

const int& SsdField::P() const
{ return m_P; }

const int& SsdField::Q() const
{ return m_Q; }

const int &SsdField::numFineValues() const
{ return m_numFineValues; }

const float* SsdField::c_fineDistanceValue() const
{ return m_fineDistance.c_data(); }

const Vector3F *SsdField::c_coarseNormalValue() const
{ return m_coarseNormal.c_value(); }
	
const Vector3F *SsdField::c_fineNormalValue() const
{ return m_fineNormal.c_data(); }

float SsdField::delta() const
{ return cellSize() / (1<<(m_Q - m_P)); }

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
	expandAabbByAabb(b, d);
}

const float *SsdField::aabb() const
{ return m_bbox.data(); }

void SsdField::verbose() const
{
	const int d = 1<<(m_Q - m_P);
	const int d1 = d + 1;
	const int d3 = d1 * d1 * d1;
	float nec = ((float)numFineValues()) / d3 / numCells() * 100;
	std::cout << " SsdField "<<m_P<<" "<<m_Q<<" "<<numFineValues()
	<<" "<< nec << " percent "
	<<"\n box "<<fieldBox()
	<<" cell_size "<<originCellSize()[3] << " delta " << delta()
	<<"\n aabb "<<m_bbox;
}

}

}