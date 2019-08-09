/*
 *  SsdfBuilder.h
 *  aloe
 *
 *  T sample
 *  Tv value
 *  P is begin level
 *  Q is end level Q > P
 *  Tr is rule
 *
 *  root node is a cubic field of resolution D = 1<<P
 *  each non-empty cell is a cubic field of resolution  
 *  D1 = 1<<(Q-P)
 *
 *  Created by jian zhang on 4/19/19.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SSDF_BUILDER_H
#define ALO_SSDF_BUILDER_H

#include <graph/AdaptiveHexagonDistance.h>
#include <svf/SvfBuilder.h>
#include <sdb/L2Tree.h>

namespace alo {

namespace sdf {

template<typename T, typename Tv, int P, int Q, typename Tr>
class SsdfBuilder : public svf::SvfBuilder<T, Tv, P, Q, Tr> {

typedef svf::SvfBuilder<T, Tv, P, Q, Tr> SvfBuilderTyp;
typedef svf::SvfBuilder<T, Vector3F, P, Q, Tr> Vec3BuilderTyp;

	AdaptiveHexagonDistance<T> m_hexa;
/// distance fields, last one is root, rest is Q cells
typedef sds::CubicField<float> DistanceFieldTyp;
	std::vector<DistanceFieldTyp* > m_uniformDistances;
/// surface normal fields
typedef sds::CubicField<Vector3F> NormalFieldTyp;
	std::vector<NormalFieldTyp* > m_uniformNormals;
/// ind to p cell, -1 if empty
	int m_cellInd[(1<<P)*(1<<P)*(1<<P)];
/// offset to p cell data, -1 if empty
	int m_cellOffset[(1<<P)*(1<<P)*(1<<P)];
	int m_currentCellOffset;
	
public:

	SsdfBuilder();
	~SsdfBuilder();
	
	void build(sds::SpaceFillingVector<T>* samples,
				Tr& rule);
/// Tf is ssdf			
	template<typename Tf>
	void save(Tf& field, Tr& rule);
	
	const AdaptiveHexagonDistance<T> &distanceField() const;
	
protected:

private:
					
	void clearUniformDistances();
    void clearNormals();
	
/// sample at lp cell corners
	void buildRoot(Tr& rule);
/// resample each lp cell containts front
	void buildPCells(Tr& rule);
	
	bool buildPCell(const int coord, const int* range,
				const sds::SpaceFillingVector<T>& src,
				Tr& rule);
/// search nodes on front 				
	int findFarNode(const int coord, Tr& rule) const;
/// all nodes in P cell is negative
	bool isPCellInside(const int coord, Tr& rule) const;
				
};

template<typename T, typename Tv, int P, int Q, typename Tr>
SsdfBuilder<T, Tv, P, Q, Tr>::SsdfBuilder()
{}

template<typename T, typename Tv, int P, int Q, typename Tr>
SsdfBuilder<T, Tv, P, Q, Tr>::~SsdfBuilder()
{ 
    clearUniformDistances(); 
    clearNormals();
}

template<typename T, typename Tv, int P, int Q, typename Tr>
void SsdfBuilder<T, Tv, P, Q, Tr>::clearUniformDistances()
{ 
	const int n = m_uniformDistances.size();
	for(int i=0;i<n;++i) {
		delete m_uniformDistances[i];
	}
	m_uniformDistances.clear();
	
/// clear cell ind
	const int nc = SvfBuilderTyp::NumPCells();
	for(int i=0;i<nc;++i) {
		m_cellInd[i] = -1;
	}
	for(int i=0;i<nc;++i) {
		m_cellOffset[i] = -1;
	}
	m_currentCellOffset = 0;
}

template<typename T, typename Tv, int P, int Q, typename Tr>
void SsdfBuilder<T, Tv, P, Q, Tr>::clearNormals()
{
    const int n = m_uniformNormals.size();
	for(int i=0;i<n;++i) {
		delete m_uniformNormals[i];
	}
	m_uniformNormals.clear();
}

template<typename T, typename Tv, int P, int Q, typename Tr>
void SsdfBuilder<T, Tv, P, Q, Tr>::build(sds::SpaceFillingVector<T>* samples,
								Tr& rule)
{
	SvfBuilderTyp::clearSamples();
	
/// finest
	SvfBuilderTyp::aggregateLevel(SvfBuilderTyp::levelSample(Q+1), *samples, Q+1, rule);
	
/// to P
	for(int i=Q;i>=P;--i) {
		SvfBuilderTyp::aggregateLevel(SvfBuilderTyp::levelSample(i), SvfBuilderTyp::levelSample(i+1), i, rule);
	}
	
	for(int i=P+1;i<=Q;++i) {
		SvfBuilderTyp::levelSample(i). template createSFCAtLevel<Tr>(rule, i);
	}
	
	SvfBuilderTyp::levelSample(Q). template createSFCAtLevel<Tr>(rule, P);
	SvfBuilderTyp::levelSample(Q+1). template createSFCAtLevel<Tr>(rule, P);
	
/// coarsest
	m_hexa. template divideCell<Tr>(0, rule, 0, P);

/// to Q
/// l7 graph could have over half a million edges
	for(int j=P;j<Q;++j) {
		if(j==Q-1) m_hexa.beginGuardCells();
        
		const sds::SpaceFillingVector<T>& curs = SvfBuilderTyp::levelSample(j);
		const int n = curs.size();
		for(int i=0;i<n;++i) {
            //m_hexa. template divideCell<Tr>(curs[i]._key, rule, j, j+1);
            rule.touchCellsAtLevel((const float *)&curs[i]._pos, j, .0625f);
            const int &nt = rule.numTouchedCells();
            for(int t=0;t<nt;++t) {
                const int &kt = rule.touchedCell(t);
                m_hexa. template divideCell<Tr>(kt, rule, j, j+1);
            }
		}
		if(j==Q-1) m_hexa. template endGuardCells<Tr>(rule, P, j);
		
	}
	
	m_hexa.finishCells(rule);
	
	m_hexa. template computeDistance<T, Tr>(*samples, Q, rule);
	
	clearUniformDistances();
    clearNormals();
	
	buildPCells(rule);
	buildRoot(rule);
}

template<typename T, typename Tv, int P, int Q, typename Tr>
const AdaptiveHexagonDistance<T> &SsdfBuilder<T, Tv, P, Q, Tr>::distanceField() const
{ return m_hexa; }

template<typename T, typename Tv, int P, int Q, typename Tr>
void SsdfBuilder<T, Tv, P, Q, Tr>::buildRoot(Tr& rule)
{
	DistanceFieldTyp* rootField = new DistanceFieldTyp;
	const int d = 1<<P;
	rootField->create(d);
	float orih[4];
	rule.getDomainOrigin(orih);
	orih[3] = rule.deltaAtLevel(P);
	rootField->setOriginCellSize(orih);
    
    NormalFieldTyp *rootNml = new NormalFieldTyp;
    rootNml->create(d);
	rootNml->setOriginCellSize(orih);
	
	const int n = (d+1)*(d+1)*(d+1);
	int* ks = new int[n];
	rule.getLevelGrid(ks, 0, 0, P);
	
	for(int i=0;i<n;++i) {
		int j = m_hexa.findNode(ks[i]);
		if(j>-1) {
			rootField->value()[i] = m_hexa.nodes()[j].val;
            
            const T &nodeV = m_hexa.nodeValue(j);
            rootNml->value()[i] = nodeV._nml;
		} else {
			std::cout<<"\n WARNING cannot find node ";
			rule.printCoord(ks[i]);
		}
	}
	
	delete[] ks;
	
	m_uniformDistances.push_back(rootField);
    m_uniformNormals.push_back(rootNml);
}

template<typename T, typename Tv, int P, int Q, typename Tr>
void SsdfBuilder<T, Tv, P, Q, Tr>::buildPCells(Tr& rule)
{	
	const sds::SpaceFillingVector<T>& lqs = SvfBuilderTyp::levelSample(Q+1);
	const int n = lqs.size();
	
	int range[2];
	range[0] = 0;
	int k = lqs[0]._key;
	for(int i=1;i<n;++i) {
		if(lqs[i]._key != k) {
			range[1] = i;
			
			buildPCell(k, range, lqs, rule);
				
			k = lqs[i]._key;
			range[0] = i;
		}
	}
	range[1] = n;
	
	buildPCell(k, range, lqs, rule);
}

template<typename T, typename Tv, int P, int Q, typename Tr>
bool SsdfBuilder<T, Tv, P, Q, Tr>::buildPCell(const int coord, const int* range,
				const sds::SpaceFillingVector<T>& src,
				Tr& rule)
{
	//int iFar = findFarNode(coord, rule);
	//if(iFar < 0) return false;
	if(isPCellInside(coord, rule)) return false;
	
/// key for Q-P grid
	const int d = 1<<(Q - P);
	const int d3 = (d+1)*(d+1)*(d+1);
	int* ks = new int[d3];
	
	DistanceFieldTyp* cellFld = new DistanceFieldTyp;
	static const float corih[4] = {0.f,0.f,0.f,rule.deltaAtLevel(P)};
	cellFld->create(1);
	cellFld->setOriginCellSize(corih);
	
    
    NormalFieldTyp *cellNml = new NormalFieldTyp;
    cellNml->create(1);
	cellNml->setOriginCellSize(corih);
	
	sdb::L2Tree<int, int, 512, 1024> visited;
/// to Q-1
	for(int l=P;l<Q;++l) {
		
		rule.getLevelGrid(ks, coord, P, l-P);
		const int dl = 1<<(l-P);
		const int d3l = (dl+1)*(dl+1)*(dl+1);
		
		for(int i=0;i<d3l;++i) {
		
			if(visited.find(ks[i]))
				continue;
				
			int ind = m_hexa.findNode(ks[i]);
			if(ind < 0)
				continue;
				
			cellFld->value()[i] = m_hexa.getNodeDistance(ind);
            
            const T &nodeV = m_hexa.nodeValue(ind);
            cellNml->value()[i] = nodeV._nml;
			
			visited.insert(ks[i], 0);
		}
		
		cellFld->upSample();
        cellNml->upSample();
	}
	
	const int cellCount = m_uniformDistances.size();
	const int iCell = rule.computeCellInd(coord, P);
	m_cellInd[iCell] = cellCount;
	m_cellOffset[iCell] = m_currentCellOffset;
	m_currentCellOffset += cellFld->numValues();
	
	m_uniformDistances.push_back(cellFld);
    m_uniformNormals.push_back(cellNml);
	
	delete[] ks;
	
	return true;
}

template<typename T, typename Tv, int P, int Q, typename Tr>
bool SsdfBuilder<T, Tv, P, Q, Tr>::isPCellInside(const int coord, Tr& rule) const
{
	const int d = 1<<(Q - P);
	const int d3 = (d+1)*(d+1)*(d+1);
	int* ks = new int[d3];

	bool stat = true;

	for(int l=P;l<Q;++l) {
		rule.getLevelGrid(ks, coord, P, l-P);
		const int dl = 1<<(l-P);
		const int d3l = (dl+1)*(dl+1)*(dl+1);

		for(int i=0;i<d3l;++i) {
			int ind = m_hexa.findNode(ks[i]);
			if(ind < 0)
				continue;

			if(m_hexa.nodes()[ind].val > 0) {
				stat = false;
				break;
			}
		}

		if(!stat) break;
	}

	delete[] ks;
	return stat;
}

template<typename T, typename Tv, int P, int Q, typename Tr>
int SsdfBuilder<T, Tv, P, Q, Tr>::findFarNode(const int coord, Tr& rule) const
{
	const float e = rule.deltaAtLevel(Q);
	int res = -1;
	float minD = e;
/// from P to Q-2
	for(int l=P;l<Q-1;++l) {
		const int d = 1<<(l-P);
		const int d3 = (d+1)*(d+1)*(d+1);
		int* ks = new int[d3];
		
		rule.getLevelGrid(ks, coord, P, l-P);
		
		bool isLevelEmpty = true;
		for(int i=0;i<d3;++i) {
			int j = m_hexa.findNode(ks[i]);
			if(j>-1) {
				isLevelEmpty = false;
				const float& ad = m_hexa.nodes()[j].val;
				if(ad > minD) {
					minD = ad;
					res = j;
				}
			}
		}
		
		delete[] ks;
		
		if(isLevelEmpty) {
/// no more levels
			break;
		}
	}
	
	return res;
}

template<typename T, typename Tv, int P, int Q, typename Tr>
template<typename Tf>
void SsdfBuilder<T, Tv, P, Q, Tr>::save(Tf& field, Tr& rule)
{
	field.create(P, Q, m_currentCellOffset);
	float orih[4];
	rule.getDomainOrigin(orih);
	orih[3] = rule.deltaAtLevel(P);
	field.setOriginCellSize(orih);
	field.setBBox(rule.bbox());
				
	const DistanceFieldTyp* rootField = m_uniformDistances.back();
	field.copyValue(rootField);
    
    const NormalFieldTyp* rootNml = m_uniformNormals.back();
	field.copyCoarseNormalValue(rootNml);
    
	field.copyCell(m_cellOffset);
				
	const int n = SvfBuilderTyp::NumPCells();
	for(int i=0;i<n;++i) {
		const int &j = m_cellInd[i];
		if(j<0) continue;
			
		const DistanceFieldTyp *cellField = m_uniformDistances[j];
		const int &offset = m_cellOffset[i];
		
		field.copyFineDistanceValue(offset, cellField);
        
        const NormalFieldTyp *cellNml = m_uniformNormals[j];  
        field.copyFineNormalValue(offset, cellNml);
	}
}

}

}
#endif
