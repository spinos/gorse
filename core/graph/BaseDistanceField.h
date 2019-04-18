/*
 *  BaseDistanceField.h
 *  aloe
 *
 *  en.wikipedia.org/wiki/Dijkstra%27s_algorithm
 *
 *  Created by zhang on 17-1-31.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_GRAPH_BASE_DISTANCE_FIELD_H
#define ALO_GRAPH_BASE_DISTANCE_FIELD_H

#include "AGraph.h"
#include <math/Vector3F.h>
#include <deque>
#include <map>

namespace alo {

namespace sdf {
enum NodeState {
	StUnknown = 0,
	StKnown,
	StVisited,
	StFar,
	StBackGround,
	StFront = 127,
	StFront1
};

}

/// graph node carries distance and value 
template<typename T>
struct DistanceNode {
	
	T _tval;
	Vector3F pos;
	float val;
	int _origin;
	short label;
	short stat;
};

struct IDistanceEdge {

	sdb::Coord2 vi; /// ind to node
	float len; /// length of edge
	short lab;
};

template<typename T>
class BaseDistanceField : public AGraph<DistanceNode<T>, IDistanceEdge > {

public:
    BaseDistanceField();
    virtual ~BaseDistanceField();

    const T &nodeValue(const int &i) const;
	
protected:	
/// very large val
	void resetNodes(float val, sdf::NodeState lab, sdf::NodeState stat);
    void resetEdges(sdf::NodeState lab);
    void unvisitAllNodes();
/// march from known sites
/// add distance, keep origin
    void fastMarchingMethod();
/// march to front
	void marchOutside(const int& originNodeInd);
    void setFarNodeInside();
/// from marching origin
	void computeAccurateDistance();
	
    void setNodeDistance(const int & idx,
                        const float & v);
	void setEdgeFront(int va, int vb);
	
	void pushIndices(const std::vector<int> & a,
							std::vector<int> & b) const;
								
/// set i-th node distance to point ref
	void setNodeDistance2(const int& i, const Vector3F& ref);

	void setNodeTValue(const int& i, const T &x);
			
private:
/// propagate distance value
    void propagate(std::deque<int > & heap, const int & i);
	void propagateVisit(std::map<int, int > & heap, const int & i);
	void addNodeToHeap(std::deque<int>& heap, const int&x) const;
	
};

template<typename T>
BaseDistanceField<T>::BaseDistanceField()
{}

template<typename T>
BaseDistanceField<T>::~BaseDistanceField()
{}

template<typename T>
void BaseDistanceField<T>::resetNodes(float val, sdf::NodeState lab, sdf::NodeState stat)
{
    const int & n = numNodes();
	for(int i = 0;i<n;++i) {
		NodeType &d = nodes()[i];
        d.val = val;
		d._origin = -1;
		d.label = lab;
		d.stat = stat;
	}
}

template<typename T>
void BaseDistanceField<T>::resetEdges(sdf::NodeState lab)
{
    IDistanceEdge* egs = edges();
    const int& ne = numEdges();
    for(int i=0;i<ne;++i) {
        IDistanceEdge& e = egs[i];
        e.lab = lab;
    }
}

template<typename T>
void BaseDistanceField<T>::unvisitAllNodes()
{
    const int & n = numNodes();
	for(int i = 0;i<n;++i) {
		NodeType & d = nodes()[i];
        d.stat = sdf::StFar;
	}
}

template<typename T>
void BaseDistanceField<T>::fastMarchingMethod()
{
/// heap of trial
	std::deque<int> trials;
	const int n = numNodes();
	int i = 0;
	for(;i<n;++i) {
		NodeType &d = nodes()[i];
		if(d.stat == sdf::StKnown) {
/// marching start
			d._origin = i;
			propagate(trials, i);
		}
	}
	
/// for each trial
	while (trials.size() > 0) {

/// A is first in trial		
		i = trials[0];
/// from A
		propagate(trials, i);
		
/// distance is known after propagation
		nodes()[i].stat = sdf::StKnown;

/// remove A from trial
		trials.erase(trials.begin() );	

	}
}

/// A to B
template<typename T>
void BaseDistanceField<T>::propagate(std::deque<int >& heap, const int& i)
{
	const NodeType & A = nodes()[i];
	
/// for each neighbor of A
	const int& endj = edgeBegins()[i+1];
	int vj, j = edgeBegins()[i];
	for(;j<endj;++j) {
		
		int k = edgeIndices()[j];

		const IDistanceEdge & eg = edges()[k];
		
		vj = eg.vi.x;
		if(vj == i) {
			vj = eg.vi.y;
        }
            
		NodeType &B = nodes()[vj];
/// min distance to B via A
/// need eikonal approximation here		
		if(B.val > A.val + eg.len) {
			B.val = A.val + eg.len;
/// origin of marching
			B._origin = A._origin;
/// tval as well
			B._tval = A._tval;
		}
		
		if(B.stat == sdf::StUnknown) {
/// add to trial
			addNodeToHeap(heap, vj);
			
		}
	}
}

template<typename T>
void BaseDistanceField<T>::computeAccurateDistance()
{
	const int& n = numNodes();
	for(int i = 0;i<n;++i) {
		NodeType &d = nodes()[i];
			
		const int& j = d._origin;
		if(j == i)
			continue;
			
		const NodeType &nj = nodes()[j];
		d.val = GetSign(d.val) * d.pos.distanceTo(nj.pos) + nj.val;
	}
}

template<typename T>
void BaseDistanceField<T>::addNodeToHeap(std::deque<int>& heap, const int&x) const
{
	for(int i=0;i<heap.size();++i) {
		if(heap[i] == x)
			return;
	}
	heap.push_back(x);
}

template<typename T>
void BaseDistanceField<T>::setFarNodeInside()
{
	const int n = numNodes();
	for(int i = 0;i<n;++i) {
		NodeType & d = nodes()[i];
		if(d.stat == sdf::StFar) {
/// inside distance is negative
			if(d.val > 0.f)
				d.val = -d.val;
		}
	}
}

template<typename T>
void BaseDistanceField<T>::marchOutside(const int& originNodeInd)
{
    unvisitAllNodes();
	    
	int i = originNodeInd;
	
/// heap of trial
	std::map<int, int> trials;
	trials[i] = 0;
	
/// for each trial
	while (trials.size() > 0) {

/// A is first in trial		
		i = trials.begin()->first;

		nodes()[i].stat = sdf::StVisited;
/// remove A from trial
		trials.erase(trials.begin() );
		
/// from A
		propagateVisit(trials, i);
		
		//std::cout<<"\n trial n "<<trials.size();
		//std::cout.flush();
	}
	
}

template<typename T>
void BaseDistanceField<T>::propagateVisit(std::map<int, int > & heap, const int & i)
{
	const NodeType &A = nodes()[i];
	
/// for each neighbor of A
	const int endj = edgeBegins()[i+1];
	int vj, j = edgeBegins()[i];
	for(;j<endj;++j) {
		
		int k = edgeIndices()[j];

		const IDistanceEdge& eg = edges()[k];
/// do not cross front edge
		if(eg.lab > sdf::StFront1)
			continue;
		
		vj = eg.vi.x;
		if(vj == i)
			vj = eg.vi.y;
			
		NodeType &B = nodes()[vj];
		
		if(B.label > sdf::StFront1) {
/// stop
			B.stat = sdf::StVisited;
		} else {
/// do not cross front
/// exclude already visited
			if(B.stat == sdf::StFar) {
				heap[vj] = 0;
            }
		}
	}
}

template<typename T>
void BaseDistanceField<T>::setNodeDistance(const int & idx,
                        const float & v) 
{
    NodeType &d = nodes()[idx];
    if(d.stat != sdf::StKnown) {
        d.val = v;
        d.stat = sdf::StKnown;
		
    } else {
/// closer to zero
        //if(Absolute<float>(d.val) > Absolute<float>(v) ) {
/// inside first
		if(d.val > v) {
            d.val = v;
        }
    }
	
	if(d.label < sdf::StFront) {
		d.label = sdf::StFront;
	} else {
		d.label++;
	}
    
}

template<typename T>
void BaseDistanceField<T>::setEdgeFront(int va, int vb)
{
	if(va < 0 || vb < 0) {
		std::cout << "\n invalid edge index (" << va << "," << vb << ")";
		return;
	}
		
	EdgeType* eg = edge(va, vb);
	if(!eg) {
		std::cout << "\n cannot find edge (" << va << "," << vb << ")";
		return;
	}
		
	if(eg->lab < sdf::StFront)
		eg->lab = sdf::StFront;
	else
		eg->lab++;
}

template<typename T>
void BaseDistanceField<T>::pushIndices(const std::vector<int> & a,
							std::vector<int> & b) const
{
	std::vector<int>::const_iterator it = a.begin();
	for(;it!=a.end();++it) {
		b.push_back(*it);
    }
}

template<typename T>
void BaseDistanceField<T>::setNodeDistance2(const int& i, const Vector3F& ref)
{
	float d = nodes()[i].pos.distanceTo(ref);		
	setNodeDistance(i, d);
}

template<typename T>
void BaseDistanceField<T>::setNodeTValue(const int& i, const T &x)
{ nodes()[i]._tval = x; }

template<typename T>
const T &BaseDistanceField<T>::nodeValue(const int &i) const
{ return nodes()[i]._tval; }

}

#endif
