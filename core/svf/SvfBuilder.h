/*
 *  SvfBuilder.h
 *  aloe
 *
 *  T sample
 *  Tv value
 *  P begin level
 *  Q end level
 *  Tr rule
 *
 *  2 levels P and Q
 *  uniform grid resolution D = 1<<P
 *  each non-empty cell is a cubic field of resolution  
 *  D1 = 1<<(Q-P)
 *
 *  Created by jian zhang on 3/2/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SVF_BUILDER_H
#define ALO_SVF_BUILDER_H

#include <sds/SpaceFillingVector.h>
#include <sds/CubicField.h>
#include <iostream>

namespace alo {

namespace svf {

template<typename T, typename Tv, int P, int Q, typename Tr>
class SvfBuilder {

/// (P,P+1,...,Q+1)
	sds::SpaceFillingVector<T> m_lSample[Q-P+2];
/// distance fields, first one is root, rest is sparse P cells
typedef sds::CubicField<Tv> VFieldTyp;
	std::vector<VFieldTyp* > m_fields;
/// ind to p cell, -1 if empty
	int m_fieldInd[(1<<P)*(1<<P)*(1<<P)];
/// offset to p cell data, -1 if empty
	int m_fieldOffset[(1<<P)*(1<<P)*(1<<P)];
	int m_currentFieldOffset;
	
public:

	SvfBuilder();
	~SvfBuilder();
	
	void build(sds::SpaceFillingVector<T>* samples,
				Tr& rule);
/// Tf is svf			
	template<typename Tf>
	void save(Tf& field, Tr& rule);
	
/// l-th level sample [P,Q+1]
	const sds::SpaceFillingVector<T>& levelSample(int l) const;
	const sds::SpaceFillingVector<T>& finestSample() const;
	
protected:
	
	sds::SpaceFillingVector<T>& levelSample(int l);
	void clearSamples();
	void aggregateLevel(sds::SpaceFillingVector<T>& dst,
					sds::SpaceFillingVector<T>& src,
					int level,
					Tr& rule);
					
	static int NumBuildLevels();
	static int NumPCells();
	
private:
					
	void clearFields();
	
/// resample each lp cell containts front
	void buildPCells(Tr& rule);
	
	bool buildPCell(const int coord, const int ind,
				const sds::SpaceFillingVector<T>& src,
				Tr& rule);
			
};

template<typename T, typename Tv, int P, int Q, typename Tr>
int SvfBuilder<T, Tv, P, Q, Tr>::NumBuildLevels()
{ return Q - P; }

template<typename T, typename Tv, int P, int Q, typename Tr>
int SvfBuilder<T, Tv, P, Q, Tr>::NumPCells()
{ return (1<<P)*(1<<P)*(1<<P); }

template<typename T, typename Tv, int P, int Q, typename Tr>
SvfBuilder<T, Tv, P, Q, Tr>::SvfBuilder()
{}

template<typename T, typename Tv, int P, int Q, typename Tr>
SvfBuilder<T, Tv, P, Q, Tr>::~SvfBuilder()
{ clearFields(); }

template<typename T, typename Tv, int P, int Q, typename Tr>
void SvfBuilder<T, Tv, P, Q, Tr>::clearFields()
{ 
	const int n = m_fields.size();
	for(int i=0;i<n;++i) {
		delete m_fields[i];
	}
	m_fields.clear();
	
/// clear cell ind
	const int nc = NumPCells();
	for(int i=0;i<nc;++i) {
		m_fieldInd[i] = -1;
	}
	for(int i=0;i<nc;++i) {
		m_fieldOffset[i] = -1;
	}
	m_currentFieldOffset = 0;
}

template<typename T, typename Tv, int P, int Q, typename Tr>
void SvfBuilder<T, Tv, P, Q, Tr>::clearSamples()
{
	for(int i=P;i<=Q+1;++i)
		levelSample(i).clear();
}

template<typename T, typename Tv, int P, int Q, typename Tr>
void SvfBuilder<T, Tv, P, Q, Tr>::aggregateLevel(sds::SpaceFillingVector<T>& dst,
					sds::SpaceFillingVector<T>& src,
					int level,
					Tr& rule)
{
	src. template createSFCAtLevel<Tr>(rule, level);
	
	T aggr = src[0];
/// must have initial weight
	aggr._wei = 1.f;
	
	const int n = src.size();
	for(int i=1;i<n;++i) {
		
		const T& cur = src[i];
		if(cur._key != aggr._key) {
			if(aggr._wei > 1.f)
				aggr /= aggr._wei;
			dst.push_back(aggr);
			
			aggr = cur;
			aggr._wei = 1.f;
			
		} else {
			aggr += cur;
			aggr._wei += 1.f;
		}
	}
/// last one
	if(aggr._wei > 1.f)
		aggr /= aggr._wei;
	dst.push_back(aggr);
	
	std::cout<<"\n n level"<<level<<" samples "<<dst.size();
	
}

template<typename T, typename Tv, int P, int Q, typename Tr>
void SvfBuilder<T, Tv, P, Q, Tr>::build(sds::SpaceFillingVector<T>* samples,
								Tr& rule)
{
	clearSamples();
	
/// finest
	aggregateLevel(levelSample(Q+1), *samples, Q+1, rule);
	
/// to P
	for(int i=Q;i>=P;--i) {
		aggregateLevel(levelSample(i), levelSample(i+1), i, rule);
	}
	
	for(int i=P;i<=Q+1;++i) {
		levelSample(i). template createSFCAtLevel<Tr>(rule, i);
	}
	
	clearFields();
	
	buildPCells(rule);
	
}

template<typename T, typename Tv, int P, int Q, typename Tr>
const sds::SpaceFillingVector<T>& SvfBuilder<T, Tv, P, Q, Tr>::levelSample(int l) const
{ return m_lSample[l-P]; }

template<typename T, typename Tv, int P, int Q, typename Tr>
sds::SpaceFillingVector<T>& SvfBuilder<T, Tv, P, Q, Tr>::levelSample(int l)
{ return m_lSample[l-P]; }

template<typename T, typename Tv, int P, int Q, typename Tr>
const sds::SpaceFillingVector<T>& SvfBuilder<T, Tv, P, Q, Tr>::finestSample() const
{ return levelSample(Q+1); }

template<typename T, typename Tv, int P, int Q, typename Tr>
void SvfBuilder<T, Tv, P, Q, Tr>::buildPCells(Tr& rule)
{	
	const sds::SpaceFillingVector<T>& lqs = levelSample(P);
	const int n = lqs.size();
	
	for(int i=0;i<n;++i) {
		int k = lqs[i]._key;
		buildPCell(k, i, lqs, rule);
	}
}

template<typename T, typename Tv, int P, int Q, typename Tr>
bool SvfBuilder<T, Tv, P, Q, Tr>::buildPCell(const int coord, const int ind,
				const sds::SpaceFillingVector<T>& src,
				Tr& rule)
{
	VFieldTyp* cellFld = new VFieldTyp;
	
	float corih[4];
	rule.computePosition(corih, coord);
	corih[3] = rule.deltaAtLevel(P);
	
	cellFld->setOriginCellSize(corih);
	cellFld->setResolution(1);
	
/// P
	cellFld->setAllValues(src[ind]._col);
	
	cellFld->upSample();
	
	int kRange[2];
	if(ind<1)
		kRange[0] = -1;
	else
		kRange[0] = src[ind-1]._key;
		
	if(ind < src.size() - 1)
		kRange[1] = src[ind+1]._key;
	else
		kRange[1] = (1<<31)-1;
		
	int lRange[2];
	
	float* weight = new float[Tr::GetGridNumValues(Q-P)];
	
/// to Q-1
	for(int l=P+1;l<Q;++l) {
	
		const sds::SpaceFillingVector<T>& lSrc = levelSample(l+1);
		lSrc.searchSFC(lRange, kRange);
		
		memset(weight, 0, Tr::GetGridNumValues(l-P)<<2);
		
		for(int i=lRange[0];i<=lRange[1];++i) {
			cellFld->addValueAt(weight, lSrc[i]._col, (const float*)&lSrc[i]._pos);
		}
		
		cellFld->divide(weight);
		
		cellFld->upSample();
	}

/// Q
	const sds::SpaceFillingVector<T>& qSrc = levelSample(Q+1);
	qSrc.searchSFC(lRange, kRange);
	
	memset(weight, 0, Tr::GetGridNumValues(Q-P)<<2);
	
	for(int i=lRange[0];i<=lRange[1];++i) {
		cellFld->addValueAt(weight, qSrc[i]._col, (const float*)&qSrc[i]._pos);
	}
	
	cellFld->divide(weight);
	
	delete[] weight;
	
	const int cellCount = m_fields.size();
	const int iCell = rule.computeCellInd(coord, P);
	m_fieldInd[iCell] = cellCount;
	m_fieldOffset[iCell] = m_currentFieldOffset;
	m_currentFieldOffset += cellFld->storageSize();
	
	m_fields.push_back(cellFld);
	
	return true;
}

template<typename T, typename Tv, int P, int Q, typename Tr>
template<typename Tf>
void SvfBuilder<T, Tv, P, Q, Tr>::save(Tf& field, Tr& rule)
{
	field.create(P, Q, m_currentFieldOffset);
	float orih[4];
	rule.getDomainOrigin(orih);
	orih[3] = rule.deltaAtLevel(P);
	field.setOriginCellSize(orih);
	
	field.copyCellOffset(m_fieldOffset);
	
	const int n = NumPCells();
	for(int i=0;i<n;++i) {
		int j = m_fieldInd[i];
		if(j<0)
			continue;
			
		const VFieldTyp* cellField = m_fields[j];
		const int offset = m_fieldOffset[i];
		
		field.copyCellValue(offset, (const char*)cellField->c_value(), cellField->dataSize() );
	}
	
}

}

}
#endif
