/*
 *  AdaptiveHexagonDistance.h
 *  aloe
 *
 *  node at cell corners
 *
 *  Created by jian zhang on 3/2/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_ADAPTIVE_HEXAGON_DISTANCE_H
#define ALO_ADAPTIVE_HEXAGON_DISTANCE_H

#include <graph/BaseDistanceField.h>
#include <sds/HashedIndex.h>
#include <sds/SpaceFillingVector.h>
#include <math/miscfuncs.h>
#include <vector>
#include <deque>
#include <iostream>

namespace alo {

namespace sdf {

class AdaptiveHexagonDistance : public BaseDistanceField {

	struct CellCorners {
		int _key[8];
	};
	
	struct CellDesc {
		int _key; /// to origin
		int _span; /// by level
		
		const bool operator==(const CellDesc & another) const
		{
			return (_key == another._key && _span == another._span);
		}
		
		const bool operator<(const CellDesc & another) const
		{ 
			if(_span < another._span)
				return true;
			if(_span > another._span)
				return false;
			return _key < another._key;
		}
		
		const bool operator>(const CellDesc & another) const
		{
			if(_span > another._span)
				return true;
			if(_span < another._span)
				return false;
			return _key > another._key;
		}
		
		friend std::ostream& operator<<(std::ostream &output, const CellDesc & p)
		{
			output << " cell ("<<p._key<<","<<p._span<<") ";
			return output;
		}
	};
	
	sdb::L3Tree<CellDesc, int, 2048, 512, 1024 > m_cellsToAdd;
	sdb::L3Tree<CellDesc, int, 2048, 512, 1024 > m_cellsToDivide;
	
	sds::HashedIndex m_nodeHash;
/// remove parent cell edges after subdiv
	sdb::L3Tree<sdb::Coord2, int, 2048, 512, 1024 > m_obslEdgeVs;
	bool m_enableGuard;
	
public:
	
	AdaptiveHexagonDistance();
	virtual ~AdaptiveHexagonDistance();
	
/// add level1 cells at coord at level0 by rule
	template<typename Tr>
	void divideCell(int kcoord, Tr& rule, int level0, int level1);
/// connecting all cells	
	template<typename Tr>
	void finishCells(Tr& rule);
/// all level cells contain samples has distance	
	template<typename T, typename Tr>
	void computeDistance(const sds::SpaceFillingVector<T>& samples, 
					const int level,
					Tr& rule);
/// same level across front
	void beginGuardCells();
	template<typename Tr>
	void endGuardCells(Tr& rule, int levelBegin, int level1);
	
/// index by key
	int findNode(int k) const;
/// i-th node
	float getNodeDistance(int i) const;
	Vector3F getNodePosition(int i) const;

/// closest distance to sample for ind-th node	
/// keep sign
	template<typename T>
	float resampleDistance(const int& ind, 
				const int* range,
				const sds::SpaceFillingVector<T>& src); 

protected:
	template<typename Tr>
	void buildGraph(const int& nv,
					Tr& rule);
					
	template<typename Tr>
	int firstEmptyCellInd(Tr& rule);

private:

	template<typename Tr>
	void divideFirstCellInQueue(std::deque<CellDesc>& q,
						Tr& rule, int levelBegin);
	
	void setNodeDistance2(const int& i, const Vector3F& ref);
	
};

template<typename Tr>
void AdaptiveHexagonDistance::divideCell(int kcoord, Tr& rule, int level0, int level1)
{
	const int nchild = 1<<((level1-level0)*3);
	CellDesc* tmp = new CellDesc[nchild];
	
	rule. template getLevelCells<CellDesc>(tmp, kcoord, level0, level1);
	
	for(int i=0;i<nchild;++i) {
		m_cellsToAdd.insert(tmp[i]);
	}
	
	delete[] tmp;
	
	if(level0 < 1) 
		return;
		
/// remove parent cell edges
	sdb::Coord2 ke[12];
	CellDesc ci;
	ci._key = kcoord;
	ci._span = 1<<(10-level0);
	rule. template computeKeyToCellEdges<CellDesc, sdb::Coord2 >(ke, ci);
	
	for(int j=0;j<12;++j) {
		
		sdb::Coord2 c = ke[j].ordered();
		m_obslEdgeVs.insert(c);
		
	}
	
	if(!m_enableGuard)
		return;
	
/// add ng guard cells to divide list
	CellDesc gcells[6];
	int ng = rule. template getGuardCells<CellDesc>(gcells, kcoord, level0, level0);
	for(int i=0;i<ng;++i) {
		m_cellsToDivide.insert(gcells[i]);
	}
}

template<typename Tr>
void AdaptiveHexagonDistance::finishCells(Tr& rule)
{
	const int n = m_cellsToAdd.size();
	std::cout<<"\n finish cells n "<<n;
	m_nodeHash.create(n<<1);
	
	int kn[8];
	m_cellsToAdd.begin();
	while(!m_cellsToAdd.end() ) {
	
		const CellDesc& ci = m_cellsToAdd.key();
		rule. template computeKeyToCellCorners<CellDesc>(kn, ci);
		
		for(int j=0;j<8;++j) {
			int l = m_nodeHash.insertKey(kn[j]);
			if(l<0) {
				std::cout<<"\n WARNING cannot insert "<<kn[j];
			}
		}
		
		m_cellsToAdd.next();
	}
	
	int nv = m_nodeHash.finish();
	std::cout<<"\n n node "<<nv;
	buildGraph(nv, rule);
	
	m_cellsToAdd.clear();
}

template<typename Tr>
void AdaptiveHexagonDistance::buildGraph(const int& nv,
										Tr& rule)
{
	sdb::Sequence<sdb::Coord2> edgeMap;
	
	sdb::Coord2 ke[12];
	
	m_cellsToAdd.begin();
	while(!m_cellsToAdd.end() ) {
	
		const CellDesc& ci = m_cellsToAdd.key();
		rule. template computeKeyToCellEdges<CellDesc, sdb::Coord2 >(ke, ci);
		
		for(int j=0;j<12;++j) {
			
			if(m_obslEdgeVs.findKey(ke[j].ordered() ) )
				continue;
				
			int v1 = findNode(ke[j].x);
			if(v1 < 0) {
				std::cout<<"\n WARNING cannot find edge "<<ke[j].x;
				continue;
			}
			int v2 = findNode(ke[j].y);
			if(v2 < 0) {
				std::cout<<"\n WARNING cannot find edge "<<ke[j].y;
				continue;
			}
			
			sdb::Coord2 c = sdb::Coord2(v1, v2).ordered();
				
			edgeMap.insert(c);
			
		}
		
		m_cellsToAdd.next();
	}
	
	m_obslEdgeVs.clear();
	
	std::map<int, std::vector<int> > vvemap;
	
	int c = 0;
	edgeMap.begin();
	while(!edgeMap.end() ) {
	
		int v0 = edgeMap.key().x;
		vvemap[v0].push_back(c);
		
		int v1 = edgeMap.key().y;
		vvemap[v1].push_back(c);
		
		c++;
		edgeMap.next();
	}
	
	std::vector<int> edgeBegins;
	std::vector<int> edgeInds;
	
	int nvve = 0;
	std::map<int, std::vector<int> >::iterator it = vvemap.begin();
	for(;it!=vvemap.end();++it) {
		edgeBegins.push_back(nvve);
		
		pushIndices(it->second, edgeInds);
		nvve += (it->second).size();
		
		it->second.clear();
	}
	
	int ne = edgeMap.size();
	int ni = edgeInds.size();
	std::cout<<"\n ne "<<ne;
	BaseDistanceField::create(nv, ne, ni);
	
	DistanceNode * dst = nodes();
	
	const int* tableKs = m_nodeHash.table();
	const int* tableIs = m_nodeHash.index();
	const int& ts = m_nodeHash.tableSize();
	for(int i=0;i<ts;++i) {
		const int& k = tableKs[i];
		if(k > -1) {
			float* p = (float*)&dst[tableIs[i]].pos;
			rule.computePosition(p, k);
		}
	}
	
	extractEdges(&edgeMap);
	extractEdgeBegins(edgeBegins);
	extractEdgeIndices(edgeInds);
    
    vvemap.clear();
	edgeBegins.clear();
	edgeInds.clear();
	
	calculateEdgeLength();
	std::cout.flush();
}

template<typename T, typename Tr>
void AdaptiveHexagonDistance::computeDistance(const sds::SpaceFillingVector<T>& samples, 
								const int level,
								Tr& rule)
{
	resetNodes(1e20f, sdf::StBackGround, sdf::StUnknown);
	resetEdges(sdf::StBackGround);
	
	int closestV, touchE[3];
	float minD;
	CellCorners ac;
	const int n = samples.size();
	for(int i=0;i<n;++i) {
		
		const Vector3F& sp = samples[i]._pos;
		
		rule.getLevelCellAt(ac, (const float*)&sp, level);
		
		minD = 1e20f;
		closestV = -1;
		
		for(int j=0;j<8;++j) {
			const int v = findNode(ac._key[j]);
			if(v<0) {
				ac._key[j] = -1;
				//std::cout<<"\n WARNING cannot find cell of "<<i
				//		<<" at level "<<level;
				//rule.printCoord(ac._key[j]);
				continue;
			} 
			
			ac._key[j] = v;
			float d = getNodePosition(v).distance2To(sp);
			if(minD > d) {
				minD = d;
				closestV = j;
			}
			
		}
		
		if(closestV > -1) {
			setNodeDistance2(ac._key[closestV], sp);
			rule.GetCellCornersConnectedToCorner(touchE, closestV);
			setEdgeFront(ac._key[closestV], ac._key[touchE[0]]);
			setEdgeFront(ac._key[closestV], ac._key[touchE[1]]);
			setEdgeFront(ac._key[closestV], ac._key[touchE[2]]);
		}
	}
	
	const int iFar = firstEmptyCellInd(rule);
	
	fastMarchingMethod();
	marchOutside(iFar);
	setFarNodeInside();
	computeAccurateDistance();
	
}

template<typename Tr>
int AdaptiveHexagonDistance::firstEmptyCellInd(Tr& rule)
{
	int iter = 0;
	int coord = rule.firstEmptySite(iter);
	while(rule.hasEmptySite(iter) ) {
		
		int k = findNode(coord);
		if(k>-1) {
			if(nodes()[k].stat == sdf::StUnknown)
				return k;
				
		} else {
			std::cout<<"\n WARNING cannot find empty "<<coord;
		}
		
		coord = rule.nextEmptySite(iter);
	}
	
	return 0;
}
	
template<typename Tr>
void AdaptiveHexagonDistance::endGuardCells(Tr& rule, int levelBegin, int level1)
{		
	std::deque<CellDesc> guardCellQueue;
	
	m_cellsToDivide.begin();
	while(!m_cellsToDivide.end() ) {
	
		const CellDesc& ci = m_cellsToDivide.key();
		guardCellQueue.push_back(ci);
		
		m_cellsToDivide.next();
	}
	
	while(guardCellQueue.size() > 0) {
	
		divideFirstCellInQueue(guardCellQueue, rule, levelBegin);
		
	}
	m_enableGuard = false;
}

template<typename Tr>
void AdaptiveHexagonDistance::divideFirstCellInQueue(std::deque<CellDesc>& q,
						Tr& rule, int levelBegin)
{
	const CellDesc& ci = q.front();
	
	int level0 = rule.computeLevelBySpan(ci._span);
	
	CellDesc tmp[8];
	
	rule. template getLevelCells<CellDesc>(tmp, ci._key, level0, level0+1);
	
	for(int i=0;i<8;++i) {
		m_cellsToAdd.insert(tmp[i]);
	}
	
	sdb::Coord2 ke[12];
	rule. template computeKeyToCellEdges<CellDesc, sdb::Coord2 >(ke, ci);
	
	for(int j=0;j<12;++j) {

		sdb::Coord2 c = ke[j].ordered();
		m_obslEdgeVs.insert(c);
		
	}
	
	if(level0 > levelBegin) {
/// coarser level
		CellDesc gcells[6];
		int ng = rule. template getGuardCells<CellDesc>(gcells, ci._key, level0, level0-1);
		for(int i=0;i<ng;++i) {
			q.push_back(gcells[i]);
		}
	}
	
	q.pop_front();
}

template<typename T>
float AdaptiveHexagonDistance::resampleDistance(const int& ind, 
				const int* range,
				const sds::SpaceFillingVector<T>& src)
{
	NodeType& nj = nodes()[ind];
	for(int i=range[0];i<range[1];++i) {
	
		Vector3F dv = nj.pos - src[i]._pos;
		float d = dv.length();

		if(Absolute<float>(nj.val) > d ) {
			nj.val = d * GetSign(nj.val);
			
		}
	
	}
	
	return nj.val;
}

}


}

#endif
