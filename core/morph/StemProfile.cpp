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
m_nodeAngle(2.349f),
m_axilAngle(.79f),
m_bendingLimit(.09f)
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

void StemProfile::setCurrentSegment(const float &x)
{ m_currentSegment = x; }

const int &StemProfile::segmentsPerSeason() const
{ return m_segmentsPerSeason; }

const float &StemProfile::nodeAngle() const
{ return m_nodeAngle; }

float StemProfile::segmentAngle() const
{ return m_nodeAngle / (float)m_segmentsPerSeason; }

const float &StemProfile::axilAngle() const
{ return m_axilAngle; }

const float &StemProfile::bendingLimit() const
{ return m_bendingLimit; }

const float &StemProfile::currentSegment() const
{ return m_currentSegment; }

}

}
