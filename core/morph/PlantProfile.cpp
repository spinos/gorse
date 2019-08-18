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
m_age(5),
m_growSpeed(1.04f, .04f),
m_branchProbability(.9f),
m_seasonalFactor(1.f)
{}

PlantProfile::~PlantProfile()
{}

void PlantProfile::setRootPosition(const Vector3F &q)
{ m_pos = q; }

void PlantProfile::setGrowDirections(const Vector3F &v0, const Vector3F &vm)
{ m_v0 = v0; m_vm = vm; }

void PlantProfile::setAge(const int &x)
{ m_age = x; }

void PlantProfile::setGrowSpeed(const Float2 &q)
{ m_growSpeed = q; }

void PlantProfile::setBranchProbability(const float &x)
{ m_branchProbability = x; }

void PlantProfile::setSeasonalFactor(const float &x)
{ m_seasonalFactor = x; }

const Vector3F &PlantProfile::rootPosition() const
{ return m_pos; }

const Vector3F &PlantProfile::initialGrowDirection() const
{ return m_v0; }

const Vector3F &PlantProfile::generalGrowDirection() const
{ return m_vm; }

const int &PlantProfile::age() const
{ return m_age; }

const Float2 &PlantProfile::growSpeed() const
{ return m_growSpeed; }

const float &PlantProfile::branchProbability() const
{ return m_branchProbability; }

Matrix33F PlantProfile::getRootRotation() const
{ return TubularProfile::calculateFrame0(m_v0, m_vm); }

Vector3F PlantProfile::getGrowVector() const
{ return m_vm.normal() * (m_growSpeed.x * m_seasonalFactor); }

}

}
