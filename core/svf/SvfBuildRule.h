/*
 *  SvfBuildRule.h
 *  aloe
 *
 *  Created by jian zhang on 2/13/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SVF_BUILD_RULE_H
#define ALO_SVF_BUILD_RULE_H

#include <sds/SpaceFillingVector.h>

namespace alo {

namespace svf {

template<typename Tc>
class SvfBuildRule {

/// space filling curve
	Tc* m_sfc;

public:

	SvfBuildRule(Tc* sfc);
	
	int computeKey(const float* p) const;
	int computeKeyAtLevel(const float* p, int level) const;
	int computeLevelBySpan(const int& s) const;
	void computePosition(float* p, int coord) const;
	int computeCellInd(int coord, int level) const;
	float deltaAtLevel(int level) const;
	void getDomainOrigin(float* p) const;
	
	template<typename Tcell>
	void computeKeyToCellCorners(int* c, const Tcell& coord) const;
	
	template<typename Tcell, typename Tedge>
	void computeKeyToCellEdges(Tedge* e, const Tcell& coord) const;
	
	template<typename Tcell>
	void getLevelCells(Tcell* cells, int coord, int level0, int level1) const;
	
	template<typename Tcell>
	void getLevelCellByCoord(Tcell& cell, const int coord, int level) const;

	template<typename Tcell>
	void getLevelCellAt(Tcell& cell, const float* p, int level) const;
/// find level0-1 neighbors
	template<typename Tcell>
	int getGuardCells(Tcell* gcells, int coord, int level0, int level1) const;
	
	void getEightCellVsAt(int* vs, const float* p, int level) const;
/// keys to uniform grid of order p
/// by coord and level
/// ks[(d+1)^3] when d <- 1<<p
	void getLevelGrid(int* ks, int coord, int level, int p);
	
	void printCoord(int coord) const;
	
	static void PrintOriginCellSize(const float* orih);
	static void GetCellGrid(float* p, const float* cellBox, int d);
	static int GetGridNumValues(int d);
	
protected:

	const Tc* spaceFillingCurve() const;

private:
	
};

template<typename Tc>
SvfBuildRule<Tc>::SvfBuildRule(Tc* sfc)
{ m_sfc = sfc; }

template<typename Tc>
const Tc* SvfBuildRule<Tc>::spaceFillingCurve() const
{ return m_sfc; }

template<typename Tc>
int SvfBuildRule<Tc>::computeKey(const float* p) const
{ return m_sfc->computeKey(p); }

template<typename Tc>
int SvfBuildRule<Tc>::computeKeyAtLevel(const float* p, int level) const
{ return m_sfc->computeKeyAtLevel(p, level); }

template<typename Tc>
void SvfBuildRule<Tc>::computePosition(float* p, int coord) const
{ m_sfc->decodeKey(p, coord); }

template<typename Tc>
float SvfBuildRule<Tc>::deltaAtLevel(int level) const
{ return m_sfc->_h * (1<<(10-level)); }

template<typename Tc>
void SvfBuildRule<Tc>::getDomainOrigin(float* p) const
{ memcpy(p, m_sfc->_origin, 12); }

template<typename Tc>
int SvfBuildRule<Tc>::computeLevelBySpan(const int& s) const
{
	return m_sfc->computeLevelBySpani(s);
}

template<typename Tc>
template<typename Tcell, typename Tedge>
void SvfBuildRule<Tc>::computeKeyToCellEdges(Tedge* e, const Tcell& coord) const
{
	int kn[8];
	computeKeyToCellCorners(kn, coord);
	
	for(int i=0;i<12;++i) {
		e[i].x = kn[TwelveHexagonEdges[i][0]];
		e[i].y = kn[TwelveHexagonEdges[i][1]];
	}
}	

template<typename Tc>
template<typename Tcell>
void SvfBuildRule<Tc>::computeKeyToCellCorners(int* c, const Tcell& coord) const
{
	int u0[3];
	m_sfc->decodeKey(u0, coord._key);
	int u[3];
	for(int k=0;k<2;++k) {
		u[2] = u0[2] + k * coord._span;
		
		for(int j=0;j<2;++j) {
			u[1] = u0[1] + j * coord._span;
			
			for(int i=0;i<2;++i) {
				u[0] = u0[0] + i * coord._span;
				
				c[k*4+j*2+i] = m_sfc->computeKey(u[0],   u[1],   u[2]);
				
			}
		}
	}
}

template<typename Tc>
void SvfBuildRule<Tc>::getLevelGrid(int* ks, int coord, int level, int p)
{
	int u0[3];
	if(level < 1) {
		u0[0] = u0[1] = u0[2] = 0;
	} else {
		m_sfc->decodeKey(u0, coord);
	}
	
	const int d = 1<<p;
	const int h = 1<<(10-level-p);
	int count = 0;
	int u[3];
	for(int k=0;k<=d;++k) {
		u[2] = u0[2] + k * h;
		
		for(int j=0;j<=d;++j) {
			u[1] = u0[1] + j * h;
			
			for(int i=0;i<=d;++i) {
				u[0] = u0[0] + i * h;
				
				ks[count] = m_sfc->computeKey(u[0], u[1], u[2]);
				count++;
			}
		}
	}
}

template<typename Tc>
template<typename Tcell>
void SvfBuildRule<Tc>::getLevelCells(Tcell* cells, int coord, int level0, int level1) const
{
	int u0[3];
	if(level0 < 1) {
		u0[0] = u0[1] = u0[2] = 0;
	} else {
		m_sfc->decodeKey(u0, coord);
	}
	
	int count = 0;
	const int d = 1<<(level1 - level0);
	const int h = 1<<(10-level1);
	Tcell ac;
	int u[3];
	for(int k=0;k<d;++k) {
		u[2] = u0[2] + k * h;
		
		for(int j=0;j<d;++j) {
			u[1] = u0[1] + j * h;
			
			for(int i=0;i<d;++i) {
				u[0] = u0[0] + i * h;
				
				ac._key = m_sfc->computeKey(u[0],   u[1],   u[2]);
				ac._span = h;
				
				cells[count] = ac;
				count++;
			}
		}
	}
}

template<typename Tc>
template<typename Tcell>
int SvfBuildRule<Tc>::getGuardCells(Tcell* gcells, int coord, int level0, int level1) const
{
	int u0[3];
	m_sfc->decodeKey(u0, coord);
	
	const int h = 1<<(10-level0);
	const int hh = h>>1;
	u0[0] += hh;
	u0[1] += hh;
	u0[2] += hh;
	const int h1 = 1<<(10-level1);
	
	int count = 0;
	int u1[3];
	
	const int kc = m_sfc->computeKeyAtLevel(u0, level1);
	
	for(int i=0;i<6;++i) {
		u1[0] = u0[0] + h * SixNeighborOffsets[i][0];
		u1[1] = u0[1] + h * SixNeighborOffsets[i][1];
		u1[2] = u0[2] + h * SixNeighborOffsets[i][2];
		const int knei = m_sfc->computeKeyAtLevel(u1, level1);
		if(knei != kc) {
			Tcell& ac = gcells[count];
			ac._key = knei;
			ac._span = h1;
			count++;
		}
	}
	
	return count;
}	

template<typename Tc>
template<typename Tcell>
void SvfBuildRule<Tc>::getLevelCellByCoord(Tcell& cell, const int coord, int level) const
{
	int u[3];
	m_sfc->decodeKey(u, coord);
	const int h = 1<<(10-level);
	cell._key[0] = m_sfc->computeKey(u[0], u[1], u[2]);
	cell._key[1] = m_sfc->computeKey(u[0]+h, u[1], u[2]);
	cell._key[2] = m_sfc->computeKey(u[0], u[1]+h, u[2]);
	cell._key[3] = m_sfc->computeKey(u[0]+h, u[1]+h, u[2]);
	cell._key[4] = m_sfc->computeKey(u[0], u[1], u[2]+h);
	cell._key[5] = m_sfc->computeKey(u[0]+h, u[1], u[2]+h);
	cell._key[6] = m_sfc->computeKey(u[0], u[1]+h, u[2]+h);
	cell._key[7] = m_sfc->computeKey(u[0]+h, u[1]+h, u[2]+h);
}

template<typename Tc>
template<typename Tcell>
void SvfBuildRule<Tc>::getLevelCellAt(Tcell& cell, const float* p, int level) const
{
	int coord = m_sfc->computeKeyAtLevel(p, level);
	getLevelCellByCoord(cell, coord, level);
}

template<typename Tc>
void SvfBuildRule<Tc>::getEightCellVsAt(int* vs, const float* p, int level) const
{
	const int coord = m_sfc->computeKeyAtLevel(p, level);
	int u0[3];
	m_sfc->decodeKey(u0, coord);
	int h = 1<<(10-level-1);
	int u1[3];
	for(int k=0;k<3;++k) {
		u1[2] = u0[2] + h * k;
		
		for(int j=0;j<3;++j) {
			u1[1] = u0[1] + h * j;
			
			for(int i=0;i<3;++i) {
				u1[0] = u0[0] + h * i;
				
				vs[k*9+j*3+i] = m_sfc->computeKey(u1[0], u1[1], u1[2]);
			}
		}
	}
}	

template<typename Tc>
void SvfBuildRule<Tc>::printCoord(int coord) const
{ 
	int u[3];
	m_sfc->decodeKey(u, coord);
	std::cout<<" u("<<u[0]<<","<<u[1]<<","<<u[2]<<") ";
}

template<typename Tc>
int SvfBuildRule<Tc>::computeCellInd(int coord, int level) const
{
	int u[3];
	m_sfc->decodeKey(u, coord);
	u[0] = u[0]>>(10-level);
	u[1] = u[1]>>(10-level);
	u[2] = u[2]>>(10-level);
	return (u[2]<<(level+level)) + (u[1]<<level) + u[0];
}

template<typename Tc>
void SvfBuildRule<Tc>::PrintOriginCellSize(const float* orih)
{
	std::cout<<" origin("<<orih[0]<<","<<orih[1]<<","<<orih[2]<<") h "<<orih[3]<<" ";
}

template<typename Tc>
void SvfBuildRule<Tc>::GetCellGrid(float* p, const float* cellBox, int d)
{
	int d1 = d+1;
	int d2 = d1*d1;
	const float h = (cellBox[3] - cellBox[0]) / (float)d;
	
	float u[3];
	for(int k=0;k<d1;++k) {
		u[2] = cellBox[2] + h * k;
		
		for(int j=0;j<d1;++j) {
			u[1] = cellBox[1] + h * j;
			
			for(int i=0;i<d1;++i) {
				u[0] = cellBox[0] + h * i;
				
				int c = 3*(k*d2 + j*d1 + i);
				memcpy(&p[c], u, 12);
				
			}
		}
	}
}

template<typename Tc>
int SvfBuildRule<Tc>::GetGridNumValues(int d)
{
	int d1 = (1<<d)+1;
	return d1*d1*d1;
}

}

}

#endif