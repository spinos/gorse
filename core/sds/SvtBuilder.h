/*
 *  SvtBuilder.h
 *  aloe
 *
 *  Created by zhang on 18-2-19.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SDS_SVT_BUILDER_H
#define ALO_SDS_SVT_BUILDER_H

#include "SpaceFillingVector.h"
#include <deque>
#include <iostream>

namespace alo {

namespace sds {

template<typename T, int N>
class VoxelTreeBuilder {

    T m_subNode[1<<(N+N+N)];
    
	SpaceFillingVector<T> m_nodes;
/// (center, half_span) of root node
	float m_coord[4];
	
public:

	VoxelTreeBuilder();
	~VoxelTreeBuilder();
	
	template<typename Ts, typename Tr>
	void build(const SpaceFillingVector<Ts>& samples, 
                const SpaceFillingVector<Ts>& fronts, 
				Tr& rule);
				
	template<typename Tt, typename Tr>
	void save(Tt* traverser, Tr& rule) const;
	
protected:

private:

	template<typename Ts, typename Tr>
	void subdivideNode(std::deque<T>& buildQueue, 
				T& parentNode,
				const SpaceFillingVector<Ts>& samples, 
				const SpaceFillingVector<Ts>& fronts, 
				Tr& rule);

	template<typename Ts, typename Tr>
	void buildFirstNodeInQueue(std::deque<T>& buildQueue, 
				const SpaceFillingVector<Ts>& samples, 
				const SpaceFillingVector<Ts>& fronts, 
				Tr& rule);
                
    void computeSubNodeCoord(T* subNode, const T& parentNode);
	
};

template<typename T, int N>
VoxelTreeBuilder<T, N>::VoxelTreeBuilder()
{}

template<typename T, int N>
VoxelTreeBuilder<T, N>::~VoxelTreeBuilder()
{ m_nodes.clear(); }

template<typename T, int N>
template<typename Ts, typename Tr>
void VoxelTreeBuilder<T, N>::build(const SpaceFillingVector<Ts>& samples, 
				const SpaceFillingVector<Ts>& fronts, 
				Tr& rule)
{
	T rootNode;
	rule. template buildRoot<T, Ts>(rootNode, m_coord, samples);
    
    Tr::PrintCoord(rootNode._coord);
    Tr::PrintRange(rootNode._range);
		
/// stack waiting to build
	std::deque<T> buildQueue;
	
    subdivideNode<Ts, Tr>(buildQueue, rootNode, samples, fronts, rule);

	while(!buildQueue.empty() ) {
		
		buildFirstNodeInQueue<Ts, Tr>(buildQueue, samples, fronts, rule);
	}
	
	const int n = m_nodes.size();
	
/// child0 at level+N has same key as parent at level
/// use level 10 key
	for(int i=0;i<n;++i) {
		m_nodes[i]._key = rule.computeFinestKey(m_nodes[i]._coord);
		m_nodes[i]._ind = i;
	}
    
    std::cout<<"\n n voxel "<<n;

}

template<typename T, int N>
template<typename Ts, typename Tr>
void VoxelTreeBuilder<T, N>::subdivideNode(std::deque<T>& buildQueue, 
				T& parentNode,
				const SpaceFillingVector<Ts>& samples, 
				const SpaceFillingVector<Ts>& fronts, 
				Tr& rule)
{
	computeSubNodeCoord(m_subNode, parentNode);
    
    rule. template computeSubNodeKeys <T, N> (m_subNode, parentNode);
    
    const int n = (1<<N)*(1<<N)*(1<<N);
	for(int i=0;i<n;++i) {
        
		int state = rule. template spawnNode<T, Ts>(m_subNode, i, parentNode, 
                                    samples, fronts);
		if(!state)
			continue;
		
		buildQueue.push_back(m_subNode[i]);

	}
}

template<typename T, int N>
void VoxelTreeBuilder<T, N>::computeSubNodeCoord(T* subNode, const T& parentNode)
{
    const float& halfSpan = parentNode._coord[3];
    float origin[3];
    origin[0] = parentNode._coord[0] - halfSpan;
    origin[1] = parentNode._coord[1] - halfSpan;
    origin[2] = parentNode._coord[2] - halfSpan;
    
    const int d = 1<<N;
    const float hh = halfSpan / (float)d;
    const float h = hh * 2.f;
    const int childLevel = parentNode._level + N;
    
    int l=0;
    for(int k=0;k<d;++k) {
        for(int j=0;j<d;++j) {
            for(int i=0;i<d;++i) {
            
                T& curSubNode = subNode[l];
                
                curSubNode._coord[0] = origin[0] + h * (i + .5f);
                curSubNode._coord[1] = origin[1] + h * (j + .5f);
                curSubNode._coord[2] = origin[2] + h * (k + .5f);
                curSubNode._coord[3] = hh;
                curSubNode._level = childLevel;
                
                l++;
            }
        }
    }
}

template<typename T, int N>
template<typename Ts, typename Tr>
void VoxelTreeBuilder<T, N>::buildFirstNodeInQueue(std::deque<T>& buildQueue, 
				const SpaceFillingVector<Ts>& samples, 
				const SpaceFillingVector<Ts>& fronts, 
				Tr& rule)
{
	T& node = buildQueue.front();

	if(rule.endSubdivide(node) ) {
        rule. template buildLeafNode<T>(node, samples, fronts);
	} else {	
		subdivideNode<Ts, Tr>(buildQueue, node, samples, fronts, rule);
		node._dataInd = -1;
	}
	m_nodes.push_back(node);
	
	buildQueue.pop_front();
}

template<typename T, int N>
template<typename Tt, typename Tr>
void VoxelTreeBuilder<T, N>::save(Tt* traverser,
                                    Tr& rule) const
{
    memcpy(traverser->coord(), m_coord, 16);
    traverser->setMaxLevel(rule.maxLevel() );
    
	const int n = m_nodes.size();
	traverser->createNumNodes(n);
						
	for(int i=0;i<n;++i) {
        traverser->insertKey(m_nodes[i]._key);
	}
    
    std::cout<<"\n n index "<<traverser->finish();
    
    for(int i=0;i<n;++i) {
        const T& ni = m_nodes[i];
        if(ni._dataInd < 0)
            continue;
            
        int j = traverser->findNode(ni._key);
        if(j<0) {
            std::cout<<"\n WARNING cannot find node "<<ni._key;
            continue;
        }
        
        traverser->saveData(j, rule.data(ni._dataInd) );
	}
    std::cout<<"\n n data "<<rule.numNodes();
    
}

}

}

#endif
