/* 
 *  PlantProfile.cpp
 *  gorse
 *
 *  2019/8/17
 */

#include "PlantProfile.h"
#include <tbl/TubularProfile.h>

namespace alo {

namespace morph {

PlantProfile::PlantProfile() :
m_pos(0.f, -.1f, 0.f),
m_v0(0.f, 1.f, 0.f),
m_vm(0.f, 1.f, 0.f),
m_age(8),
m_stopAge(8, 4),
m_growSpeed(1.f, .05f),
m_mainAxisGrowSpeedRatio(1.f, 1.f),
m_branchProbability(.9f),
m_seasonalFactor(1.f),
m_minBranchSeason(1),
m_stopProbability(.17f),
m_resourceRatio(.7f),
m_currentSeason(0)
{}

PlantProfile::~PlantProfile()
{}

void PlantProfile::setRootPosition(const Vector3F &q)
{ m_pos = q; }

void PlantProfile::setGrowDirections(const Vector3F &v0, const Vector3F &vm)
{ m_v0 = v0; m_vm = vm; }

void PlantProfile::setAge(const int &x)
{ m_age = x; }

void PlantProfile::setStopAge(const Int2 &x)
{ m_stopAge = x; }

void PlantProfile::setGrowSpeed(const Float2 &q)
{ m_growSpeed = q; }

void PlantProfile::setBranchProbability(const float &x)
{ m_branchProbability = x; }

void PlantProfile::setSeasonalFactor(const float &x)
{ m_seasonalFactor = x; }

void PlantProfile::setMinBranchSeason(const int &x)
{ m_minBranchSeason = x; }

void PlantProfile::setStopProbability(const float &x)
{ m_stopProbability = x; }

void PlantProfile::setResourceRatio(const float &x)
{ m_resourceRatio = x; }

void PlantProfile::setCurrentSeason(const int &x)
{ m_currentSeason = x; }

const Vector3F &PlantProfile::rootPosition() const
{ return m_pos; }

const Vector3F &PlantProfile::initialGrowDirection() const
{ return m_v0; }

const Vector3F &PlantProfile::generalGrowDirection() const
{ return m_vm; }

const int &PlantProfile::age() const
{ return m_age; }

const Int2 &PlantProfile::stopAge() const
{ return m_stopAge; }

const Float2 &PlantProfile::growSpeed() const
{ return m_growSpeed; }

const float &PlantProfile::branchProbability() const
{ return m_branchProbability; }

const int &PlantProfile::minBranchSeason() const
{ return m_minBranchSeason; }

const float &PlantProfile::stopProbability() const
{ return m_stopProbability; }

const float &PlantProfile::resourceRatio() const
{ return m_resourceRatio; }

const int &PlantProfile::currentSeason() const
{ return m_currentSeason; }

Matrix33F PlantProfile::getRootRotation() const
{ return TubularProfile::calculateFrame0(m_v0, m_vm); }

Vector3F PlantProfile::getGrowVector() const
{ return m_vm.normal() * (m_growSpeed.x * m_seasonalFactor); }

void PlantProfile::setMainAxisGrowSpeedRatio(const Float2 &x)
{ m_mainAxisGrowSpeedRatio = x; }

const Float2 &PlantProfile::mainAxisGrowSpeedRatio() const
{ return m_mainAxisGrowSpeedRatio; }

}

}
