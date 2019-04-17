/*
 *  UniformHexagonDistance.cpp
 *  aloe
 *
 *  Created by jian zhang on 2/10/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#include "UniformHexagonDistance.h"
#include <math/Vector3F.h>
#include <math/BoundingBox.h>
#include <math/Float4.h>
#include <math/miscfuncs.h>
#ifdef LINUX
#include <stdlib.h>
#include <string.h>
#endif
namespace alo {

UniformHexagonDistance::UniformHexagonDistance()
{ memset(m_dim, 0, 64); }

UniformHexagonDistance::~UniformHexagonDistance()
{}

const int& UniformHexagonDistance::numCells() const
{ return m_dim[7]; }

bool UniformHexagonDistance::isEmpty() const
{ return numCells() < 1; }

const int* UniformHexagonDistance::dimension() const
{ return m_dim; }

const float* UniformHexagonDistance::originCellSize() const
{ return m_originSize; }

const float& UniformHexagonDistance::cellSize() const
{ return m_originSize[3]; }
	
void UniformHexagonDistance::create(int M, int N, int P,
				const float* boxOrigin,
				const float& cellSize)
{	
	m_dim[0] = M;
	m_dim[1] = N;
	m_dim[2] = P;
	m_dim[3] = M+1;
	m_dim[4] = N+1;
	m_dim[5] = P+1;
	m_dim[6] = m_dim[3] * m_dim[4];
	m_dim[7] = m_dim[0] * m_dim[1] * m_dim[2];
	setOriginAndCellSize(boxOrigin, cellSize);
	buildGraph();
}

void UniformHexagonDistance::setOriginAndCellSize(const float* boxOrigin,
				const float& cellSize)
{
	m_originSize[0] = boxOrigin[0];
	m_originSize[1] = boxOrigin[1];
	m_originSize[2] = boxOrigin[2];
	m_originSize[3] = cellSize;
	m_originSize[4] = 1.f / cellSize;
}

int UniformHexagonDistance::nodeInd(int i, int j, int k) const
{ return k*m_dim[6] + j*m_dim[3] + i;}

void UniformHexagonDistance::buildGraph()
{
	sdb::Sequence<sdb::Coord2> edgeMap;
	
	for(int k=0;k<=dimension()[2];++k) {
		for(int j=0;j<=dimension()[1];++j) {
			for(int i=0;i<dimension()[0];++i) {
				
				sdb::Coord2 c = sdb::Coord2(nodeInd(i,j,k),
										nodeInd(i+1,j,k) ).ordered();
				if(!edgeMap.findKey(c) ) {
					edgeMap.insert(c);
				}
			}
		}
	}
	
	for(int k=0;k<=dimension()[2];++k) {
		for(int i=0;i<=dimension()[0];++i) {
			for(int j=0;j<dimension()[1];++j) {
				
				sdb::Coord2 c = sdb::Coord2(nodeInd(i,j,k),
										nodeInd(i,j+1,k) ).ordered();
				if(!edgeMap.findKey(c) ) {
					edgeMap.insert(c);
				}
			}
		}
	}
	
	for(int j=0;j<=dimension()[1];++j) {
		for(int i=0;i<=dimension()[0];++i) {
			for(int k=0;k<dimension()[2];++k) {
				
				sdb::Coord2 c = sdb::Coord2(nodeInd(i,j,k),
										nodeInd(i,j,k+1) ).ordered();
				if(!edgeMap.findKey(c) ) {
					edgeMap.insert(c);
				}
			}
		}
	}
	
	std::map<int, std::vector<int> > vvemap;
	
	int c = 0;
	edgeMap.begin();
	while(!edgeMap.end() ) {
	
		int v0 = edgeMap.key().x;
		vvemap[v0].push_back(c);
		
		int v1 = edgeMap.key().y;
		vvemap[v1].push_back(c);
		
		c++;
		edgeMap.next();
	}
	
	std::vector<int> edgeBegins;
	std::vector<int> edgeInds;
	
	int nvve = 0;
	std::map<int, std::vector<int> >::iterator it = vvemap.begin();
	for(;it!=vvemap.end();++it) {
		edgeBegins.push_back(nvve);
		
		pushIndices(it->second, edgeInds);
		nvve += (it->second).size();
		
		it->second.clear();
	}
    
    const int nv = dimension()[3] * dimension()[4] * dimension()[5];
	int ne = edgeMap.size();
	int ni = edgeInds.size();
	BaseDistanceField::create(nv, ne, ni);
	
	updateNodePosition();
	
	extractEdges(&edgeMap);
	extractEdgeBegins(edgeBegins);
	extractEdgeIndices(edgeInds);
    
    vvemap.clear();
	edgeBegins.clear();
	edgeInds.clear();
	
}

void UniformHexagonDistance::updateNodePosition()
{
	DistanceNode * dst = nodes();
	float u[3];
	for(int k=0;k<=dimension()[2];++k) {
		u[2] = originCellSize()[2] + cellSize() * k;
		
		for(int j=0;j<=dimension()[1];++j) {
			u[1] = originCellSize()[1] + cellSize() * j;
		
			for(int i=0;i<=dimension()[0];++i) {
				u[0] = originCellSize()[0] + cellSize() * i;
		
				dst[nodeInd(i,j,k)].pos.set(u[0],u[1],u[2]);
			}
		}
	}
	setAllEdgeLength(cellSize() );
}

const float& UniformHexagonDistance::getDistanceAt(const Vector3F& u) const
{
	int j = nodeInd(u);
	return nodes()[j].val;
}

int UniformHexagonDistance::nodeInd(const Vector3F& u) const
{
	float wx = (u.x - originCellSize()[0]) * originCellSize()[4];
	if(wx<0) wx=0;
	if(wx>dimension()[0]) wx=dimension()[0];
	int i = wx;
	if(wx - i > .5) i++;
	
	int wy = (u.y - originCellSize()[1]) * originCellSize()[4];
	if(wy<0) wy=0;
	if(wy>dimension()[1]) wy=dimension()[1];
	int j = wy;
	if(wy - j > .5) j++;
	
	int wz = (u.z - originCellSize()[2]) * originCellSize()[4];
	if(wz<0) wz=0;
	if(wz>dimension()[2]) wz=dimension()[2];
	int k = wz;
	if(wz - k > .5) k++;
	
	return nodeInd(i,j,k);
}

void UniformHexagonDistance::setCellDistance(const Vector3F& u, const float& d)
{
	int i = (u.x - originCellSize()[0]) * originCellSize()[4];
	if(i<0) i=0;
	if(i > dimension()[0]-1) i = dimension()[0]-1;
	
	int j = (u.y - originCellSize()[1]) * originCellSize()[4];
	if(j<0) j=0;
	if(j > dimension()[1]-1) j = dimension()[1]-1;
	
	int k = (u.z - originCellSize()[2]) * originCellSize()[4];
	if(k<0) k=0;
	if(k > dimension()[2]-1) k = dimension()[2]-1;
	
	setNodeDistanceTo(nodeInd(i,j,k), u);
	setNodeDistanceTo(nodeInd(i+1,j,k), u);
	setNodeDistanceTo(nodeInd(i,j+1,k), u);
	setNodeDistanceTo(nodeInd(i+1,j+1,k), u);
	setNodeDistanceTo(nodeInd(i,j,k+1), u);
	setNodeDistanceTo(nodeInd(i+1,j,k+1), u);
	setNodeDistanceTo(nodeInd(i,j+1,k+1), u);
	setNodeDistanceTo(nodeInd(i+1,j+1,k+1), u);
}

void UniformHexagonDistance::setNodeDistanceTo(int i, const Vector3F& u)
{ setNodeDistance(i, nodes()[i].pos.distanceTo(u) ); }

}
