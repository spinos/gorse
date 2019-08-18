/* 
 *  StemProfile.cpp
 *  gorse
 *
 *  2019/8/17
 */

#include "StemProfile.h"

namespace alo {

namespace morph {

StemProfile::StemProfile() :
m_segmentsPerSeason(3),
m_nodeAngle(.8f),
m_axilAngle(.7f),
m_bendingLimit(.021f)
{}

StemProfile::~StemProfile()
{}

void StemProfile::setSegmentsPerSeason(const int &x)
{ m_segmentsPerSeason = x; }

void StemProfile::setNodeAngle(const float &x)
{ m_nodeAngle = x; }

void StemProfile::setAxilAngle(const float &x)
{ m_axilAngle = x; }

void StemProfile::setBendingLimit(const float &x)
{ m_bendingLimit = x; }

const int &StemProfile::segmentsPerSeason() const
{ return m_segmentsPerSeason; }

const float &StemProfile::nodeAngle() const
{ return m_nodeAngle; }

const float &StemProfile::axilAngle() const
{ return m_axilAngle; }

const float &StemProfile::bendingLimit() const
{ return m_bendingLimit; }

}

}
