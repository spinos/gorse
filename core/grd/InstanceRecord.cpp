/*
 *  InstanceRecord.cpp
 *  gorse
 *
 *  2019/5/17
 */

#include "InstanceRecord.h"

namespace alo {

namespace grd {

InstanceRecord::InstanceRecord() : m_numObjects(0),
m_numInstances(0)
{}
	
InstanceRecord::~InstanceRecord()
{}

void InstanceRecord::create(const int &numObjects, const int &numInstances)
{
	m_tms.resetBuffer(numInstances);
	m_inds.resetBuffer(numInstances);
	m_numInstances = numInstances;
	m_numObjects = numObjects;
}

void InstanceRecord::setAabb(const float *b)
{ m_bbox = BoundingBox(b); }

Matrix44F *InstanceRecord::tms()
{ return m_tms.data(); }

int *InstanceRecord::inds()
{ return m_inds.data(); }

const Matrix44F *InstanceRecord::c_tms() const
{ return m_tms.c_data(); }

const int *InstanceRecord::c_inds() const
{ return m_inds.c_data(); }

const int &InstanceRecord::numInstances() const
{ return m_numInstances; }

const int &InstanceRecord::numObjects() const
{ return m_numObjects; }

bool InstanceRecord::isEmpty() const
{ return m_numObjects < 1 || m_numInstances < 1; }

void InstanceRecord::calcAabb()
{
    m_bbox.reset();
    for(int i=0;i<m_numInstances;++i) {
        Vector3F ti = m_tms[i].getTranslation();
        m_bbox.expandBy(ti);
    }
}

float InstanceRecord::getMinimumCellSize() const
{ return sqrt(m_bbox.area() * .0017f); }

void InstanceRecord::getAabb(float *b) const
{ memcpy(b, m_bbox.data(), 24); }

void InstanceRecord::verbose() const
{
	std::cout << "\n InstanceRecord n obj "<<numObjects() << " n instance " << numInstances()
        << "\n bbox " << m_bbox << " min cell size " << getMinimumCellSize();
        
}

} /// end of grd

} /// end of alo
