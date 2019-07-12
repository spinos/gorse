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
/// level
	int m_P;
/// 8 corners of cell
	int m_cornerKeys[8];
	
public:

	SparseFieldBuildRule(Tc *sfc);
	
/// level
	void setP(int x);
	void setBBox(const BoundingBox &b);
	
	const int &P() const;
    const BoundingBox &bbox() const;
    void getOriginCellSize(float *y) const;
    float getCellSize() const;
/// encode position at level
	int computeKey(const float* p) const;
	void computeCellCornerKeys(const int &cellKey);
/// decode key
	void computePosition(float* p, int coord) const;

	const int *cellCornerKeys() const;

	const Tc *c_curve() const;

protected:

	int cellDelta() const;

private:
	
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

template<typename Tc>
void SparseFieldBuildRule<Tc>::getOriginCellSize(float *y) const
{
	y[0] = m_bbox.getMin(0);
	y[1] = m_bbox.getMin(1);
	y[2] = m_bbox.getMin(2);
	const int d = cellDelta();
	y[3] = m_sfc->delta() * d;
}

template<typename Tc>
float SparseFieldBuildRule<Tc>::getCellSize() const
{ 
	const int d = cellDelta();
	return m_sfc->delta() * d;
}

template<typename Tc>
int SparseFieldBuildRule<Tc>::computeKey(const float* p) const
{ return m_sfc->computeKeyAtLevel(p, m_P); }

template<typename Tc>
void SparseFieldBuildRule<Tc>::computePosition(float* p, int coord) const
{ m_sfc->decodeKey(p, coord); }

template<typename Tc>
void SparseFieldBuildRule<Tc>::computeCellCornerKeys(const int &cellKey)
{
	m_cornerKeys[0] = cellKey;
	int u[3];
	m_sfc->decodeKey(u, cellKey);

	const int d = cellDelta();
	m_cornerKeys[1] = m_sfc->computeKey(u[0] + d, u[1],     u[2]);
	m_cornerKeys[2] = m_sfc->computeKey(u[0]    , u[1] + d, u[2]);
	m_cornerKeys[3] = m_sfc->computeKey(u[0] + d, u[1] + d, u[2]);
	m_cornerKeys[4] = m_sfc->computeKey(u[0]    , u[1],     u[2] + d);
	m_cornerKeys[5] = m_sfc->computeKey(u[0] + d, u[1],     u[2] + d);
	m_cornerKeys[6] = m_sfc->computeKey(u[0]    , u[1] + d, u[2] + d);
	m_cornerKeys[7] = m_sfc->computeKey(u[0] + d, u[1] + d, u[2] + d);
}

template<typename Tc>
int SparseFieldBuildRule<Tc>::cellDelta() const
{ return 1<<(10 - m_P); }

template<typename Tc>
const int *SparseFieldBuildRule<Tc>::cellCornerKeys() const
{ return m_cornerKeys; }

template<typename Tc>
const Tc *SparseFieldBuildRule<Tc>::c_curve() const
{ return m_sfc; }

}

}

#endif
