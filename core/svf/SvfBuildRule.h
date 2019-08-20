/*
 *  SvfBuildRule.h
 *  aloe
 *
 *  2019/8/10
 */

#ifndef ALO_SVF_BUILD_RULE_H
#define ALO_SVF_BUILD_RULE_H

#include <sds/SpaceFillingVector.h>

namespace alo {

namespace svf {

template<typename Tc>
class SvfBuildRule {

/// aabb
    BoundingBox m_bbox;
    BoundingBox m_domain;
/// space filling curve
	Tc* m_sfc;
/// cell touched by sample with offset
    int m_cell27Coord[27];
    int m_numTouchedCells;

public:

	SvfBuildRule(Tc* sfc);
    
/// tight box and domain box
    void setBBox(const BoundingBox &b);
    const BoundingBox &bbox() const;
/// encode position
	int computeKey(const float* p) const;
	int computeKeyAtLevel(const float* p, int level) const;
	int computeLevelBySpan(const int& s) const;
/// decode key
	void computePosition(float* p, int coord) const;
	int computeCellInd(int coord, int level) const;
/// cell size
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
/// b[6] (low, high) from (_key, _span)
	template<typename Tcell>
	void getCellBox(float *b, const Tcell &kh) const;

	void getEightCellVsAt(int* vs, const float* p, int level) const;
/// keys to uniform grid of order p
/// by coord and level
/// ks[(d+1)^3] when d <- 1<<p
	void getLevelGrid(int* ks, int coord, int level, int p);
	
	void printCoord(int coord) const;

/// 27 cell coords
    void touchCellsAtLevel(const float *p, int level, float size);
	const int &numTouchedCells() const;
    const int &touchedCell(int i) const;

    template<typename Te>
    void getEdgeNeighborsAtLevel(Te & neis, const float *p, int level) const;

    bool isCellIntersectDomain(const Vector3F &origin, const float &span) const;
    bool isPointInsideDomain(const Vector3F &q) const;
    
/// (-1 0 +1)^3 cells centered at coord
    void getLevel27Cells(int *nei, const int coord, int level) const;
    
	static void PrintOriginCellSize(const float* orih);
/// divide box to grid of d resolution
/// p is float[n] n is 3(1+d)^3
	static void GetCellGrid(float* p, const float* cellBox, int d);
	static int GetGridNumValues(int d);
	static void PrintAabb(const float *b);
	
protected:

	const Tc* spaceFillingCurve() const;

private:
	
    static float sTwentySevenOffset[27][3];
    
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
void SvfBuildRule<Tc>::getLevel27Cells(int *nei, const int coord, int level) const
{
    int u[3];
	m_sfc->decodeKey(u, coord);
	const int h = 1<<(10-level);
    for(int i=0;i<27;++i) {
        nei[i] = m_sfc->computeKey(u[0] + h * sTwentySevenOffset[i][0], 
                                    u[1] + h * sTwentySevenOffset[i][1], 
                                    u[2] + h * sTwentySevenOffset[i][2]);
    }
}

template<typename Tc>
template<typename Tcell>
void SvfBuildRule<Tc>::getLevelCellByCoord(Tcell& cell, const int coord, int level) const
{
	int u[3];
	m_sfc->decodeKey(u, coord);
	const int h = 1<<(10-level);
	cell._key[0] = coord;
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
void SvfBuildRule<Tc>::getCellBox(float *b, const Tcell &kh) const
{
	computePosition(b, kh._key);
	const float l = m_sfc->delta() * kh._span;
	b[3] = b[0] + l;
	b[4] = b[1] + l;
	b[5] = b[2] + l;
}

template<typename Tc>
template<typename Tcell>
void SvfBuildRule<Tc>::getLevelCellAt(Tcell& cell, const float* p, int level) const
{
	int coord = m_sfc->computeKeyAtLevel(p, level);
	if(coord != cell._key[0]) 
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
	if(u[0] > 1023) u[0] = 1023;
	if(u[1] > 1023) u[1] = 1023;
	if(u[2] > 1023) u[2] = 1023;

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

template<typename Tc>
void SvfBuildRule<Tc>::setBBox(const BoundingBox &b)
{
    m_bbox = b;
    const Vector3F midP = b.center();
	const float spanL = b.getLongestDistance();
	m_sfc->setCoord(midP.x, midP.y, midP.z, spanL * .5f);

	memcpy(&m_domain, m_sfc->_origin, 12);
	m_domain.setMax(m_domain.getMin(0) + spanL, 0);
	m_domain.setMax(m_domain.getMin(1) + spanL, 1);
	m_domain.setMax(m_domain.getMin(2) + spanL, 2);
}

template<typename Tc>
const BoundingBox &SvfBuildRule<Tc>::bbox() const
{ return m_bbox; }

template<typename Tc>
void SvfBuildRule<Tc>::PrintAabb(const float *b)
{ 
	std::cout<<" aabb (("<<b[0]<<","<<b[1]<<","<<b[2]
				<<"), ("<<b[3]<<","<<b[4]<<","<<b[5]<<")) "; 
}

template<typename Tc>
float SvfBuildRule<Tc>::sTwentySevenOffset[27][3] = {
{-1.f,-1.f,-1.f},
{ 0.f,-1.f,-1.f},
{ 1.f,-1.f,-1.f},
{-1.f, 0.f,-1.f},
{ 0.f, 0.f,-1.f},
{ 1.f, 0.f,-1.f},
{-1.f, 1.f,-1.f},
{ 0.f, 1.f,-1.f},
{ 1.f, 1.f,-1.f},
{-1.f,-1.f, 0.f},
{ 0.f,-1.f, 0.f},
{ 1.f,-1.f, 0.f},
{-1.f, 0.f, 0.f},
{ 0.f, 0.f, 0.f},
{ 1.f, 0.f, 0.f},
{-1.f, 1.f, 0.f},
{ 0.f, 1.f, 0.f},
{ 1.f, 1.f, 0.f},
{-1.f,-1.f, 1.f},
{ 0.f,-1.f, 1.f},
{ 1.f,-1.f, 1.f},
{-1.f, 0.f, 1.f},
{ 0.f, 0.f, 1.f},
{ 1.f, 0.f, 1.f},
{-1.f, 1.f, 1.f},
{ 0.f, 1.f, 1.f},
{ 1.f, 1.f, 1.f}
};

template<typename Tc>
void SvfBuildRule<Tc>::touchCellsAtLevel(const float *p, int level, float size)
{
    const float offset = size * deltaAtLevel(level);
    float q[3];
    std::map<int, int> touchMap;
    for(int i=0;i<27;++i) {
        q[0] = p[0] + offset * sTwentySevenOffset[i][0];
        q[1] = p[1] + offset * sTwentySevenOffset[i][1];
        q[2] = p[2] + offset * sTwentySevenOffset[i][2];
        int x = m_sfc->computeKeyAtLevel(q, level);
        touchMap[x] = 0;
    }
    
    m_numTouchedCells = touchMap.size();

    std::map<int, int>::iterator it = touchMap.begin();
    for(int i=0;it!=touchMap.end();++it,++i) 
        m_cell27Coord[i] = it->first;
    
    touchMap.clear();
}

template<typename Tc>
const int &SvfBuildRule<Tc>::numTouchedCells() const
{ return m_numTouchedCells; }

template<typename Tc>
const int &SvfBuildRule<Tc>::touchedCell(int i) const
{ return m_cell27Coord[i]; }

template<typename Tc>
bool SvfBuildRule<Tc>::isCellIntersectDomain(const Vector3F &origin, const float &span) const
{
	const BoundingBox b(origin.x, origin.y, origin.z,
				origin.x + span, origin.y + span, origin.z + span);
	return (m_domain.intersect(b));
}

template<typename Tc>
bool SvfBuildRule<Tc>::isPointInsideDomain(const Vector3F &q) const
{ return m_domain.isPointInside(q); }

template<typename Tc>
template<typename Te>
void SvfBuildRule<Tc>::getEdgeNeighborsAtLevel(Te & neis, const float *p, int level) const
{
	int coord = m_sfc->computeKeyAtLevel(p, level);
	getLevelCellByCoord(neis, coord, level);
	float mind = 1e10f;
	int cen;
	float q[3];
	for(int i=0;i<8;++i) {
		computePosition(q, neis._key[i]);
		q[0] -= p[0];
		q[1] -= p[1];
		q[2] -= p[2];
		float d = sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2]);
		if(mind > d) {
			mind = d;
			cen = neis._key[i];
		}
	}

	neis._key[0] = cen;

	int u[3];
	m_sfc->decodeKey(u, cen);
	const int h = 1<<(10-level);
	neis._key[1] = m_sfc->computeKey(u[0]-h, u[1]  , u[2]);
	neis._key[2] = m_sfc->computeKey(u[0]+h, u[1]  , u[2]);
	neis._key[3] = m_sfc->computeKey(u[0]  , u[1]-h, u[2]);
	neis._key[4] = m_sfc->computeKey(u[0]  , u[1]+h, u[2]);
	neis._key[5] = m_sfc->computeKey(u[0]  , u[1]  , u[2]-h);
	neis._key[6] = m_sfc->computeKey(u[0]  , u[1]  , u[2]+h);

}

}

}

#endif