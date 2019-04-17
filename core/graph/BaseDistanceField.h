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

struct DistanceNode {
	
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

class BaseDistanceField : public AGraph<DistanceNode, IDistanceEdge > {

public:
    BaseDistanceField();
    virtual ~BaseDistanceField();
	
	void displayNodes() const;
	
	bool estimateNodeNormal(Vector3F& nml, int i) const;
    
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
							
							
private:
/// propagate distance value
    void propagate(std::deque<int > & heap, const int & i);
	void propagateVisit(std::map<int, int > & heap, const int & i);
	void addNodeToHeap(std::deque<int>& heap, const int&x) const;
	
};

}

#endif
