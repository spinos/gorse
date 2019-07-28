/*
 *  SsdfBuildRule.h
 *  aloe
 *
 *  Created by jian zhang on 2/13/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SSDF_BUILD_RULE_H
#define ALO_SSDF_BUILD_RULE_H

#include <svf/SvfBuildRule.h>

namespace alo {

namespace sdf {

template<typename Tc>
class SsdfBuildRule : public svf::SvfBuildRule<Tc> {

typedef svf::SvfBuildRule<Tc> SvfRuleTyp;

public:

	SsdfBuildRule(Tc* sfc);
	
	int firstEmptySite(int& iter);
	int nextEmptySite(int& iter);
	bool hasEmptySite(int& iter);
/// corners connected to corner_i
	static void GetCellCornersConnectedToCorner(int* vs, const int i);
    
    static void GetCellEdge(int *e, const int *v, const int i);
	
protected:

private:
	static int NumEmptySites;
	static int EmptySiteU[9][3];
	static int CornerEdges[8][3];
    static int sCellEdges[12][2];
	
};

template<typename Tc>
int SsdfBuildRule<Tc>::NumEmptySites = 9;

template<typename Tc>
int SsdfBuildRule<Tc>::EmptySiteU[9][3] = {
{0, 1023, 0},
{512, 1023, 0},
{1023, 1023, 0},
{0, 1023, 512},
{512, 1023, 512},
{1023, 1023, 512},
{0, 1023, 1023},
{512, 1023, 1023},
{1023, 1023, 1023}
};

///     2 -- 3
///   / |  / |
///     0 -- 1
///  6 -- 7
///  |    | /
///  4 -- 5

template<typename Tc>
int SsdfBuildRule<Tc>::CornerEdges[8][3] = {
{1, 2, 4},
{0,3,5},
{0,3,6},
{1,2,7},
{0,5,6},
{1,4,7},
{2,4,7},
{3,5,6}
};

template<typename Tc>
int SsdfBuildRule<Tc>::sCellEdges[12][2] = {
{0, 1}, {2, 3}, {4, 5}, {6, 7},
{0, 2}, {1, 3}, {4, 6}, {5, 7},
{0, 4}, {1, 5}, {2, 6}, {3, 7}
};

template<typename Tc>
SsdfBuildRule<Tc>::SsdfBuildRule(Tc* sfc) : svf::SvfBuildRule<Tc>(sfc)
{}

template<typename Tc>
void SsdfBuildRule<Tc>::GetCellCornersConnectedToCorner(int* vs, const int i)
{
	vs[0] = CornerEdges[i][0];
	vs[1] = CornerEdges[i][1];
	vs[2] = CornerEdges[i][2];
}

template<typename Tc>
int SsdfBuildRule<Tc>::firstEmptySite(int& iter)
{
	iter = 0;
	return SvfRuleTyp::spaceFillingCurve()->computeKey(EmptySiteU[iter][0], EmptySiteU[iter][1], EmptySiteU[iter][2]);
}

template<typename Tc>	
int SsdfBuildRule<Tc>::nextEmptySite(int& iter)
{
	iter++;
	return SvfRuleTyp::spaceFillingCurve()->computeKey(EmptySiteU[iter][0], EmptySiteU[iter][1], EmptySiteU[iter][2]);
}
	
template<typename Tc>
bool SsdfBuildRule<Tc>::hasEmptySite(int& iter)
{
	return iter<NumEmptySites;
}

template<typename Tc>
void SsdfBuildRule<Tc>::GetCellEdge(int *e, const int *v, const int i)
{
    e[0] = v[sCellEdges[i][0]];
    e[1] = v[sCellEdges[i][1]];
}

}

}

#endif