/*
 *  HexahedronDistanceBuildRule.h
 *
 *  2019/7/12
 */

#ifndef ALO_SGDF_HEXAHEDRON_DISTANCE_BUILD_RULE_H
#define ALO_SGDF_HEXAHEDRON_DISTANCE_BUILD_RULE_H

#include <sds/SparseFieldBuildRule.h>

namespace alo {
    
namespace gdf {

template<typename Tc>
class HexahedronDistanceBuildRule : public sds::SparseFieldBuildRule<Tc> {

	Vector3F m_originv;

public:

	HexahedronDistanceBuildRule(Tc *sfc);

	int numConnectionsPerNode() const;
/// i-th connection to node k
	int getNodeConnection(const int &k, const int &i) const;

	const Vector3F &originPoint() const;

protected:

private:
/// by lattice boltzmann method
	static int SNodeConnectionOffset[26][3];

};

template<typename Tc>
HexahedronDistanceBuildRule<Tc>::HexahedronDistanceBuildRule(Tc *sfc) : sds::SparseFieldBuildRule<Tc>(sfc),
m_originv(Vector3F::Zero)
{}

template<typename Tc>
const Vector3F &HexahedronDistanceBuildRule<Tc>::originPoint() const
{ return m_originv; }

template<typename Tc>
int HexahedronDistanceBuildRule<Tc>::numConnectionsPerNode() const
{ return 26; }

template<typename Tc>
int HexahedronDistanceBuildRule<Tc>::getNodeConnection(const int &k, const int &i) const
{
	int u[3];
	c_curve()->decodeKey(u, k);
	const int d = 1<<(10 - P());
	u[0] += SNodeConnectionOffset[i][0] * d;
	u[1] += SNodeConnectionOffset[i][1] * d;
	u[2] += SNodeConnectionOffset[i][2] * d;
	return c_curve()->computeKey(u[0], u[1], u[2]);
}

template<typename Tc>
int HexahedronDistanceBuildRule<Tc>::SNodeConnectionOffset[26][3] = {
{ 1, 0, 0}, /// 1
{-1, 0, 0},
{ 0, 1, 0},
{ 0,-1, 0},
{ 0, 0, 1},
{ 0, 0,-1},
{ 1, 1, 0}, /// 2
{-1, 1, 0},
{ 1,-1, 0},
{-1,-1, 0},
{ 1, 0, 1},
{-1, 0, 1},
{ 1, 0,-1},
{-1, 0,-1},
{ 0, 1, 1},
{ 0,-1, 1},
{ 0, 1,-1},
{ 0,-1,-1},
{ 1, 1, 1}, /// 3
{-1, 1, 1},
{ 1,-1, 1},
{-1,-1, 1},
{ 1, 1,-1},
{-1, 1,-1},
{ 1,-1,-1},
{-1,-1,-1}
};

}

}

#endif
