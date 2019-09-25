/*
 *  BaseDistanceField.h
 *  aloe
 *
 *  en.wikipedia.org/wiki/Dijkstra%27s_algorithm
 *
 *  2019/9/25
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
	StFront1,
	StFront2,
	StFront3,
	StFront4
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
class BaseDistanceField {

    AGraph<DistanceNode<T>, IDistanceEdge > m_graph;
    float m_minEdgeLen, m_maxEdgeLen;
	
public:

    void create(int nn, int ne, int ni);
    
    DistanceNode<T> *nodes();
    IDistanceEdge *edges();
    
    const DistanceNode<T> *nodes() const;
    const IDistanceEdge *edges() const;
    
    const int &numNodes() const;
    
    const float &nodeDistance(const int &i) const;
    const T &nodeValue(const int &i) const;
    const float &minEdgeLength() const;
	const float &maxEdgeLength() const;
	
    void verbose() const;
    
    typedef DistanceNode<T> NodeType;
	typedef IDistanceEdge EdgeType;
	
protected:
    void extractEdges(sdb::L3Tree<sdb::Coord2, int, 2048, 512, 1024> *a);
	void extractEdgeBegins(const std::vector<int> & a);
	void extractEdgeIndices(const std::vector<int> & a);
    void calculateEdgeLength();
	void resetNodes(float val, sdf::NodeState lab, sdf::NodeState stat);
    void resetEdges(sdf::NodeState lab);
    void unvisitAllNodes();
/// march from known sites
/// add distance, keep origin
    void fastMarchingMethod();
/// march to front
	void marchOutside(const int& originNodeInd);
    void setFarNodeInside();
/// from sample of marching origin
	void computeAccurateDistance();
/// lower node distance 
    bool setNodeDistance(const int & idx,
                        const float & v,
                        bool toAccumulateFront = true);
	void setEdgeFront(int va, int vb);
								
/// update i-th node by shorten distance to T._pos
	void setNodeDistanceTValue(const int& i, const T &ref,
						bool toAccumulateFront = true);

	void setNodeTValue(const int& i, const T &x);
/// fix distance when to-sample direction deviates from sample normal
	void fixThinSheet(const float &maxDistance);
/// known site value is negative if behind sample    
    void fixKnownBackside();

private:
/// propagate distance value
    void propagate(std::deque<int > & heap, const int & i);
	void propagateVisit(std::map<int, int > & heap, const int & i);
	void addNodeToHeap(std::deque<int>& heap, const int&x) const;
	
};

template<typename T>
void BaseDistanceField<T>::create(int nn, int ne, int ni)
{ m_graph.create(nn, ne, ni); }

template<typename T>
DistanceNode<T> *BaseDistanceField<T>::nodes()
{ return m_graph.nodes(); }

template<typename T>
IDistanceEdge *BaseDistanceField<T>::edges()
{ return m_graph.edges(); }
    
template<typename T>
const DistanceNode<T> *BaseDistanceField<T>::nodes() const
{ return m_graph.nodes(); }

template<typename T>
const IDistanceEdge *BaseDistanceField<T>::edges() const
{ return m_graph.edges(); }

template<typename T>
const int &BaseDistanceField<T>::numNodes() const
{ return m_graph.numNodes(); }

template<typename T>
const float &BaseDistanceField<T>::minEdgeLength() const
{ return m_minEdgeLen; }

template<typename T>
const float &BaseDistanceField<T>::maxEdgeLength() const
{ return m_maxEdgeLen; }

template<typename T>
void BaseDistanceField<T>::verbose() const
{ 
    m_graph.verbose(); 
    std::cout <<"\n distance field min/max edge length "<<minEdgeLength()
								<<"/"<<maxEdgeLength();
}

template<typename T>
void BaseDistanceField<T>::extractEdges(sdb::L3Tree<sdb::Coord2, int, 2048, 512, 1024> * a)
{ m_graph.extractEdges(a); }

template<typename T>
void BaseDistanceField<T>::extractEdgeBegins(const std::vector<int> & a)
{ m_graph.extractEdgeBegins(a); }
 
template<typename T>
void BaseDistanceField<T>::extractEdgeIndices(const std::vector<int> & a)
{ m_graph.extractEdgeIndices(a); }

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
    const int &ne = m_graph.numEdges();
    for(int i=0;i<ne;++i) {
        EdgeType &e = edges()[i];
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
	const int &n = numNodes();
	for(int i=0;i<n;++i) {
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
		int i = trials[0];
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
	const int& endj = m_graph.edgeBegins()[i+1];
	int vj, j = m_graph.edgeBegins()[i];
	for(;j<endj;++j) {
		
		int k = m_graph.edgeIndices()[j];

		const IDistanceEdge & eg = edges()[k];
        
/// do not cross front edge
		if(eg.lab > sdf::StFront3)
			continue;
		
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
	const int &n = numNodes();
	for(int i = 0;i<n;++i) {
		NodeType &d = nodes()[i];
		d.val = GetSign(d.val) * d.pos.distanceTo(d._tval._pos);
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
	const int &n = numNodes();
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
	const int endj = m_graph.edgeBegins()[i+1];
	int vj, j = m_graph.edgeBegins()[i];
	for(;j<endj;++j) {
		
		int k = m_graph.edgeIndices()[j];

		const EdgeType &eg = edges()[k];
/// do not cross front edge
		if(eg.lab > sdf::StFront3)
			continue;
		
		vj = eg.vi.x;
		if(vj == i)
			vj = eg.vi.y;
			
		NodeType &B = nodes()[vj];
		if(B.label > sdf::StFront3) {
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
bool BaseDistanceField<T>::setNodeDistance(const int & idx,
                        const float & v, 
                        bool toAccumulateFront) 
{
	bool changed = false;
    NodeType &d = nodes()[idx];
    if(d.stat != sdf::StKnown) {
        d.val = v;
        d.stat = sdf::StKnown;
		changed = true;
    } else {
/// closer to zero
        //if(Absolute<float>(d.val) > Absolute<float>(v) ) {
/// inside first
		if(d.val > v) {
            d.val = v;
            changed = true;
        }
    }

    if(!changed) return changed;
    if(!toAccumulateFront) return changed;
	
	if(d.label < sdf::StFront) {
		d.label = sdf::StFront;
	} else {
		d.label++;
	}
    return changed;
}

template<typename T>
void BaseDistanceField<T>::setEdgeFront(int va, int vb)
{
	if(va < 0 || vb < 0) {
		std::cout << "\n invalid edge index (" << va << "," << vb << ")";
		return;
	}
		
	IDistanceEdge* eg = m_graph.edge(va, vb);
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
void BaseDistanceField<T>::setNodeDistanceTValue(const int& i, const T &ref,
							bool toAccumulateFront)
{
	float d = nodes()[i].pos.distanceTo(ref._pos);		
	if(setNodeDistance(i, d, toAccumulateFront)) nodes()[i]._tval = ref;
}

template<typename T>
void BaseDistanceField<T>::setNodeTValue(const int& i, const T &x)
{ nodes()[i]._tval = x; }

template<typename T>
const float &BaseDistanceField<T>::nodeDistance(const int &i) const
{ return nodes()[i].val; }

template<typename T>
const T &BaseDistanceField<T>::nodeValue(const int &i) const
{ return nodes()[i]._tval; }

template<typename T>
void BaseDistanceField<T>::fixThinSheet(const float &maxDistance)
{
	const int & n = numNodes();
	for(int i = 0;i<n;++i) {
		NodeType & d = nodes()[i];
        if(d.val > maxDistance ||
        	d.val < -maxDistance) continue;

        Vector3F s2g = d.pos - d._tval._pos;
        d.val = s2g.dot(d._tval._nml);
	}
}

template<typename T>
void BaseDistanceField<T>::fixKnownBackside()
{
    const int & n = numNodes();
	for(int i = 0;i<n;++i) {
		NodeType & d = nodes()[i];
        if(d.stat != sdf::StKnown) continue;

        Vector3F s2g = d._tval._pos - d.pos;
        if(s2g.dot(d._tval._nml) > 0)
            d.val *=  -1.f;
	}
}

template<typename T>
void BaseDistanceField<T>::calculateEdgeLength()
{
	m_minEdgeLen = 1e9f;
	m_maxEdgeLen = -1e9f;
	
	const int &n = m_graph.numEdges();
	for(int i=0;i<n;++i) {
		
		EdgeType &ei = edges()[i];
		ei.len = nodes()[ei.vi.x].pos.distanceTo(nodes()[ei.vi.y].pos);
		if(ei.len < 1e-3f) std::cout << "\n WARNING zero length " << ei.len << " edge " 
			<< ei.vi.x << ":" << ei.vi.y;
		
		if(m_minEdgeLen > ei.len)
			m_minEdgeLen = ei.len;
			
		if(m_maxEdgeLen < ei.len)
			m_maxEdgeLen = ei.len;
	}
}

}

#endif
