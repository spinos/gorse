/*
 *  UniformHexagonDistance.h
 *  aloe
 *  
 *  uniform grid of M-by-N-by-P resolution
 *  nodes are stored at cell corners
 *  (M+1)x(N+1)x(P+1) nodes
 *  edges are axix-aligned and have same length
 *  
 *  Created by jian zhang on 2/10/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SDS_UNIFORM_DENSITY_H
#define ALO_SDS_UNIFORM_DENSITY_H

#include <graph/BaseDistanceField.h>
#include <sds/SpaceFillingVector.h>

namespace alo {

class UniformHexagonDistance : public BaseDistanceField {

/// (m,n,p,m+1,n+1,p+1,(m+1)*(n+1),m*n*p)
	int m_dim[8];
/// (x,y,z,h,1/h)
	float m_originSize[5];
	
public:
	
	UniformHexagonDistance();
	virtual ~UniformHexagonDistance();
	
	void create(int M, int N, int P,
				const float* boxOrigin,
				const float& cellSize);

/// reset				
	void setOriginAndCellSize(const float* boxOrigin,
				const float& cellSize);
		
	void updateNodePosition();
	
	bool isEmpty() const;
	const int& numCells() const;
	const int* dimension() const;
	const float* originCellSize() const;
	const float& cellSize() const;
	int nodeInd(int i, int j, int k) const;
/// closest to u
	int nodeInd(const Vector3F& u) const;
	
	const float& getDistanceAt(const Vector3F& u) const;
	
	template<typename T>
	void computeDistance(const sds::SpaceFillingVector<T>& src,
				const int* range, const Vector3F& uEmpty);
	
private:
	
	void buildGraph();
/// find cell of u, set 8 corners distance d
	void setCellDistance(const Vector3F& u, const float& d);
	void setNodeDistanceTo(int i, const Vector3F& u);
};

template<typename T>
void UniformHexagonDistance::computeDistance(const sds::SpaceFillingVector<T>& src,
				const int* range, const Vector3F& uEmpty)
{
	resetNodes(1e20f, sdf::StBackGround, sdf::StUnknown);
	
	for(int i=range[0];i<range[1];++i) {
		
		setCellDistance(src[i]._pos, 0.f);
	
	}
	
	fastMarchingMethod();
	/*
	int iFar = nodeInd(uEmpty);
/// visit out nodes
	marchOutside(iFar, cellSize() * 1.99f);
/// unvisited nodes are negative
	setFarNodeInside();
	*/
	computeAccurateDistance();
	
}

}

#endif
