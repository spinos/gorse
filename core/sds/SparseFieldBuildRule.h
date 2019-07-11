/*
 *  SparseFieldBuildRule.h
 *  gorse sds
 *
 *
 *  2019/7/11
 */
 
#ifndef ALO_SPARSE_FIELD_BUILD_RULE_H
#define ALO_SPARSE_FIELD_BUILD_RULE_H

#include <math/BoundingBox.h>
#include <sds/SpaceFillingVector.h>

namespace alo {
    
namespace sds {
	
template<typename Tc>
class SparseFieldBuildRule {
	
/// aabb
    BoundingBox m_bbox;
/// space filling curve
	Tc *m_sfc;
	int m_P;
	
public:

	SparseFieldBuildRule(Tc *sfc);
	
/// level
	void setP(int x);
	void setBBox(const BoundingBox &b);
	
	const int &P() const;
    const BoundingBox &bbox() const;
	
};

template<typename Tc>
SparseFieldBuildRule<Tc>::SparseFieldBuildRule(Tc *sfc) : m_P(5)
{ m_sfc = sfc; }

template<typename Tc>
void SparseFieldBuildRule<Tc>::setP(int x)
{ m_P = x; }

template<typename Tc>
void SparseFieldBuildRule<Tc>::setBBox(const BoundingBox &b)
{
    m_bbox = b;
    const Vector3F midP = b.center();
	const float spanL = b.getLongestDistance();
	m_sfc->setCoord(midP.x, midP.y, midP.z, spanL * .5f);
}

template<typename Tc>
const int &SparseFieldBuildRule<Tc>::P() const
{ return m_P; }

template<typename Tc>
const BoundingBox &SparseFieldBuildRule<Tc>::bbox() const
{ return m_bbox; }

}

}

#endif
