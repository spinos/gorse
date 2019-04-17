/*
 *  BaseDistanceField.cpp
 *  aloe
 *
 *  Created by zhang on 17-1-31.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#include "BaseDistanceField.h"
#include <math/miscfuncs.h>

namespace alo {

BaseDistanceField::BaseDistanceField()
{}

BaseDistanceField::~BaseDistanceField()
{}

void BaseDistanceField::resetNodes(float val, sdf::NodeState lab, sdf::NodeState stat)
{
    const int & n = numNodes();
	for(int i = 0;i<n;++i) {
		DistanceNode & d = nodes()[i];
        d.val = val;
		d._origin = -1;
		d.label = lab;
		d.stat = stat;
	}
}

void BaseDistanceField::resetEdges(sdf::NodeState lab)
{
    IDistanceEdge* egs = edges();
    const int& ne = numEdges();
    for(int i=0;i<ne;++i) {
        IDistanceEdge& e = egs[i];
        e.lab = lab;
    }
}

void BaseDistanceField::unvisitAllNodes()
{
    const int & n = numNodes();
	for(int i = 0;i<n;++i) {
		DistanceNode & d = nodes()[i];
        d.stat = sdf::StFar;
	}
}

void BaseDistanceField::fastMarchingMethod()
{
/// heap of trial
	std::deque<int> trials;
	const int n = numNodes();
	int i = 0;
	for(;i<n;++i) {
		DistanceNode& d = nodes()[i];
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
void BaseDistanceField::propagate(std::deque<int >& heap, const int& i)
{
	const DistanceNode & A = nodes()[i];
	
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
            
		DistanceNode& B = nodes()[vj];
/// min distance to B via A
/// need eikonal approximation here		
		if(B.val > A.val + eg.len) {
			B.val = A.val + eg.len;
/// origin of marching
			B._origin = A._origin;
		}
		
		if(B.stat == sdf::StUnknown) {
/// add to trial
			addNodeToHeap(heap, vj);
			
		}
	}
}

void BaseDistanceField::computeAccurateDistance()
{
	const int& n = numNodes();
	for(int i = 0;i<n;++i) {
		DistanceNode& d = nodes()[i];
			
		const int& j = d._origin;
		if(j == i)
			continue;
			
		const DistanceNode& nj = nodes()[j];
		d.val = GetSign(d.val) * d.pos.distanceTo(nj.pos) + nj.val;
	}
}

void BaseDistanceField::addNodeToHeap(std::deque<int>& heap, const int&x) const
{
	for(int i=0;i<heap.size();++i) {
		if(heap[i] == x)
			return;
	}
	heap.push_back(x);
}

void BaseDistanceField::setFarNodeInside()
{
	const int n = numNodes();
	for(int i = 0;i<n;++i) {
		DistanceNode & d = nodes()[i];
		if(d.stat == sdf::StFar) {
/// inside distance is negative
			if(d.val > 0.f)
				d.val = -d.val;
		}
	}
}

void BaseDistanceField::marchOutside(const int& originNodeInd)
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

void BaseDistanceField::propagateVisit(std::map<int, int > & heap, const int & i)
{
	const DistanceNode & A = nodes()[i];
	
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
			
		DistanceNode& B = nodes()[vj];
		
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

void BaseDistanceField::setNodeDistance(const int & idx,
                        const float & v) 
{
    DistanceNode& d = nodes()[idx];
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

void BaseDistanceField::setEdgeFront(int va, int vb)
{
	if(va < 0 || vb < 0)
		return;
		
	EdgeType* eg = edge(va, vb);
	if(!eg)
		return;
		
	if(eg->lab < sdf::StFront)
		eg->lab = sdf::StFront;
	else
		eg->lab++;
}

void BaseDistanceField::displayNodes() const
{
	std::cout<<"\n BaseDistanceField::displayNodes";
	
	for(int i=0;i<numNodes();++i) {
		const DistanceNode & d = nodes()[i];
		std::cout<<"\n node["<<i<<"] pos "<<d.pos
				<<" val "<<d.val;
	}
	
}

void BaseDistanceField::pushIndices(const std::vector<int> & a,
							std::vector<int> & b) const
{
	std::vector<int>::const_iterator it = a.begin();
	for(;it!=a.end();++it) {
		b.push_back(*it);
    }
}

bool BaseDistanceField::estimateNodeNormal(Vector3F& nml, int i) const
{
	if(nodes()[i].label < sdf::StFront)
		return false;
	const float& dc = nodes()[i].val;
	if(dc > 0)
		return false;
	
	bool noSignChange = true;
	const int endj = edgeBegins()[i+1];
	int vj, j = edgeBegins()[i];
/// for each neighbor of A
	for(;j<endj;++j) {
	
		int k = edgeIndices()[j];

		const IDistanceEdge & eg = edges()[k];
		
		vj = eg.vi.x;
		if(vj == i)
			vj = eg.vi.y;
			
		const DistanceNode& B = nodes()[vj];
		if(B.val > 0) {
			noSignChange = false;
			break;
		}

	}
	
	if(noSignChange)
		return false;
		
	nml.setZero();
	
	const Vector3F& cen = nodes()[i].pos;
	
	j = edgeBegins()[i];
	for(;j<endj;++j) {
		
		int k = edgeIndices()[j];

		const IDistanceEdge & eg = edges()[k];
		
		vj = eg.vi.x;
		if(vj == i)
			vj = eg.vi.y;
			
		const DistanceNode& B = nodes()[vj];
		
		if(B.val > 0)
			nml += (B.pos - cen) * B.val;
	}
	
	nml.normalize();
	return true;
}

}
