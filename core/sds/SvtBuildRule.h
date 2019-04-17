/*
 *  SvtBuildRule.h
 *  aloe
 *
 *  Created by jian zhang on 2/13/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SDS_SVT_BUILD_RULE_H
#define ALO_SDS_SVT_BUILD_RULE_H

#include <sds/SpaceFillingVector.h>

namespace alo {

namespace sds {

template<typename Tc>
class SvtBuildRule {

/// space filling curve
	const Tc* m_sfc;
/// to build
	int m_maxLevel;
	
public:

	SvtBuildRule(const Tc& sfc);
	
	void setMaxLevel(int x);
    const int& maxLevel() const;
	
/// T is svo node
/// Ts is sample
/// coord as (center, half_span)
	template<typename T, typename Ts>
	void buildRoot(T& node, float* coord,
			const SpaceFillingVector<Ts>& samples);
	
	void computeChildCoord(float* coord, const float* parentCoord, const int& i);
	int computeFinestKey(const float* coord);
    
/// key to each subnode by coord
/// sort to find key range
/// N is level increase
    template<typename T, int N>
    void computeSubNodeKeys(T* subNode, const T& parentNode);
	
/// spawn i-th child from parent
/// return false if no sample found in range
	template<typename T, typename Ts>
	bool spawnNode(T* child, const int& i, T& parent,
			const SpaceFillingVector<Ts>& samples,
            const SpaceFillingVector<Ts>& fronts);
		
	template<typename T>
	bool endSubdivide(const T& node) const;
	
	template<typename T>
	static void PrintNode(const T& node);
	
	static void PrintCoord(const float* c);
    static void PrintRange(const int* r);
	
/// builer Tb to traverser Tt
	template<typename Tt, typename Tb>
	static void SaveRootNode(Tt& t, float* rootCoord,
					const Tb& b);
					
	template<typename Tt, typename Tb>
	static void SaveNode(Tt& t, const Tb& b);
	
protected:

private:
	
};

template<typename Tc>
SvtBuildRule<Tc>::SvtBuildRule(const Tc& sfc) :
m_maxLevel(4)
{ m_sfc = &sfc; }

template<typename Tc>
void SvtBuildRule<Tc>::setMaxLevel(int x)
{ m_maxLevel = x; }

template<typename Tc>
const int& SvtBuildRule<Tc>::maxLevel() const
{ return m_maxLevel; }

template<typename Tc>
template<typename T, typename Ts>
void SvtBuildRule<Tc>::buildRoot(T& node, float* coord,
			const SpaceFillingVector<Ts>& samples)
{
	m_sfc->getBox(node._coord);
/// be the first one
	node._key = -1;
	node._level = 0;
	node._nextKey = (1<<31)-1;
	node._range[0] = 0;
	node._range[1] = samples.size();
	m_sfc->getBox(coord);
}

template<typename Tc>
template<typename T>
bool SvtBuildRule<Tc>::endSubdivide(const T& node) const
{
	bool stat = node._level >= m_maxLevel;
	return stat;
}

template<typename Tc>
void SvtBuildRule<Tc>::computeChildCoord(float* coord, const float* parentCoord, const int& i)
{ m_sfc->computeChildCoord(coord, i, parentCoord); }

template<typename Tc>
int SvtBuildRule<Tc>::computeFinestKey(const float* coord)
{ return m_sfc->computeKey(coord); }

template<typename Tc>
template<typename T, int N>
void SvtBuildRule<Tc>::computeSubNodeKeys(T* subNode, const T& parentNode)
{
    const int childLevel = parentNode._level + N;
    const int d = (1<<N)*(1<<N)*(1<<N);
    for(int i=0;i<d;++i) {
        subNode[i]._key = m_sfc->computeKeyAtLevel(subNode[i]._coord, childLevel);
    }
    
    const int last  = d - 1;
    QuickSort1::SortByKey<int, T>(subNode, 0, last);
	
	for(int i=0;i<last;++i) {
		subNode[i]._nextKey = subNode[i+1]._key;
	}
	subNode[last]._nextKey = parentNode._nextKey;
    
}

template<typename Tc>
template<typename T, typename Ts>
bool SvtBuildRule<Tc>::spawnNode(T* child, const int& i, T& parent,
			const SpaceFillingVector<Ts>& samples,
            const SpaceFillingVector<Ts>& fronts)
{
/// must constain front samples
    if(!fronts.searchSFC(child[i]._range, child[i]._key, child[i]._nextKey) )
        return false;
        
    return samples.searchSFC(child[i]._range, child[i]._key, child[i]._nextKey);
}

template<typename Tc>
template<typename T>
void SvtBuildRule<Tc>::PrintNode(const T& node)
{
	std::cout<<"  k "<<node._key
		<<"  l "<<node._level
		<<"\n    p "<<node.parentInd()
		<<" c ("<<node._ind[1]
		<<","<<node._ind[2]
		<<","<<node._ind[3]
		<<","<<node._ind[4]
		<<","<<node._ind[5]
		<<","<<node._ind[6]
		<<","<<node._ind[7]
		<<","<<node._ind[8]
		<<") d "<<node._ind[9]
		<<"  r ("<<node._range[0]
		<<","<<node._range[1]
		<<")";
}

template<typename Tc>
void SvtBuildRule<Tc>::PrintCoord(const float* c)
{ 
	std::cout<<" coord ("<<c[0]
		<<","<<c[1]
		<<","<<c[2]
		<<","<<c[3]
		<<") ";
}

template<typename Tc>
void SvtBuildRule<Tc>::PrintRange(const int* r)
{ 
	std::cout<<" range ("<<r[0]
		<<","<<r[1]
		<<") ";
}

template<typename Tc>
template<typename Tt, typename Tb>
void SvtBuildRule<Tc>::SaveRootNode(Tt& t, float* rootCoord,
						const Tb& b)
{
	memcpy(rootCoord, b._coord, 16);
	b.getChildInds(t._ind);
	t._ind[8] = -1;
	b.getLocation(t._ind[9]);
}

template<typename Tc>
template<typename Tt, typename Tb>
void SvtBuildRule<Tc>::SaveNode(Tt& t, const Tb& b)
{
	b.getChildInds(t._ind);
	b.getParent(t._ind[8]);
	b.getLocation(t._ind[9]);
}

}

}

#endif