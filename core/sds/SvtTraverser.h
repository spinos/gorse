/*
 *  SvtTraverser.h
 *  aloe
 *
 *  T is node
 *  N is subdiv order
 *
 *  Created by jian zhang on 2/15/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SDS_SVT_TRAVERSER_H
#define ALO_SDS_SVT_TRAVERSER_H

#include "HashedIndex.h"
#include <boost/scoped_array.hpp>

namespace alo {

namespace sds {

template<typename T, int N>
class SvtTraverser {

/// (center, half_span) of root node
	float m_coord[4];
    HashedIndex m_indices;
    boost::scoped_array<T> m_nodes;
	int m_numNodes;
    int m_maxLevel;
	
public:
	
	SvtTraverser();
	~SvtTraverser();
	
	float* coord();
	const float* coord() const;
	
	T& node(int i);
	const T& node(int i) const;
	
	const T* nodes() const;

    void createNumNodes(int n);
/// return location in hash table
    int insertKey(const int& k);
/// scan index return n used location
/// should be equal to n nodes
    int finish();
	
	void setMaxLevel(int x);
	const int& numNodes() const;
    const int& maxLevel() const;
    float minSubdivSpan() const;
	const int& tabelLength() const;
	const int* hashTable() const;
	const int* hashIndex() const;
/// look by k
/// return index
/// return -1 if not found
    int findNode(const int k) const;
    
    void saveData(int i, const T& x);
	
	T* nodesR();
	int* hashTableR();
	int* hashIndexR();

/// 1<<N
	static int NumSubdivPerDim();
	
	typedef T NodeType;
	
protected:

private:

};

template<typename T, int N>
SvtTraverser<T, N>::SvtTraverser() : m_numNodes(0),
m_maxLevel(0)
{}

template<typename T, int N>
SvtTraverser<T, N>::~SvtTraverser()
{}

template<typename T, int N>
void SvtTraverser<T, N>::setMaxLevel(int x)
{ m_maxLevel = x; }

template<typename T, int N>
const int& SvtTraverser<T, N>::maxLevel() const
{ return m_maxLevel; }

template<typename T, int N>
float SvtTraverser<T, N>::minSubdivSpan() const
{ return m_coord[3] / (float)(1<<m_maxLevel); }

template<typename T, int N>
void SvtTraverser<T, N>::createNumNodes(int n)
{ 
	m_numNodes = n;
	m_nodes.reset(new T[n]); 
    m_indices.create(n);
}

template<typename T, int N>
T& SvtTraverser<T, N>::node(int i)
{ return m_nodes.get()[i]; }

template<typename T, int N>
const T& SvtTraverser<T, N>::node(int i) const
{ return m_nodes.get()[i]; }

template<typename T, int N>
float* SvtTraverser<T, N>::coord()
{ return m_coord; }

template<typename T, int N>
const float* SvtTraverser<T, N>::coord() const
{ return m_coord; }

template<typename T, int N>
const T* SvtTraverser<T, N>::nodes() const
{ return m_nodes.get(); }

template<typename T, int N>
const int& SvtTraverser<T, N>::numNodes() const
{ return m_numNodes; }

template<typename T, int N>
const int& SvtTraverser<T, N>::tabelLength() const
{ return m_indices.tableSize(); }

template<typename T, int N>
const int* SvtTraverser<T, N>::hashTable() const
{ return m_indices.table(); }

template<typename T, int N>
const int* SvtTraverser<T, N>::hashIndex() const
{ return m_indices.index(); }

template<typename T, int N>
int SvtTraverser<T, N>::insertKey(const int& k)
{ return m_indices.insertKey(k); }

template<typename T, int N>
int SvtTraverser<T, N>::finish()
{ return m_indices.finish(); }

template<typename T, int N>
int SvtTraverser<T, N>::NumSubdivPerDim()
{ return 1<<N; }

template<typename T, int N>
int SvtTraverser<T, N>::findNode(const int k) const
{ return m_indices.lookupKey(k); }

template<typename T, int N>
void SvtTraverser<T, N>::saveData(int i, const T& x)
{ m_nodes[i] = x; }

template<typename T, int N>
T* SvtTraverser<T, N>::nodesR()
{ return m_nodes.get(); }

template<typename T, int N>
int* SvtTraverser<T, N>::hashTableR()
{ return m_indices.tableR(); }

template<typename T, int N>
int* SvtTraverser<T, N>::hashIndexR()
{ return m_indices.indexR(); }

}

}

#endif
