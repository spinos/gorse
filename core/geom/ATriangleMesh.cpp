/*
 *  ATriangleMesh.cpp
 *  aloe
 *
 *  Created by jian zhang on 4/25/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include "ATriangleMesh.h"

namespace alo {

ATriangleMesh::ATriangleMesh() : m_numVertices(0),
    m_numTriangles(0),
    m_numIndices(0)
{}

ATriangleMesh::~ATriangleMesh() 
{}

const int& ATriangleMesh::numVertices() const
{ return m_numVertices; }

const int& ATriangleMesh::numTriangles() const
{ return m_numTriangles; }

const int& ATriangleMesh::numIndices() const
{ return m_numIndices; }

const Vector3F* ATriangleMesh::c_positions() const
{ return m_positions.c_data(); }

const Vector3F* ATriangleMesh::c_normals() const
{ return m_normals.c_data(); }

const unsigned* ATriangleMesh::c_indices() const
{ return m_indices.c_data(); }

unsigned* ATriangleMesh::indices()
{ return m_indices.data(); } 

Vector3F* ATriangleMesh::positions()
{ return m_positions.data(); }

Vector3F* ATriangleMesh::normals()
{ return m_normals.data(); }

void ATriangleMesh::setNumVertices(int x)
{ m_numVertices = x; }

void ATriangleMesh::setNumTriangles(int x)
{
    m_numTriangles = x;
    m_numIndices = x * 3;
}

SimpleBuffer<Vector3F>& ATriangleMesh::positionBuffer()
{ return m_positions; }

SimpleBuffer<Vector3F>& ATriangleMesh::normalBuffer()
{ return m_normals; }

SimpleBuffer<unsigned>& ATriangleMesh::indexBuffer()
{ return m_indices; }

void ATriangleMesh::createTriangleMesh(int vertexCount, int triangleCount)
{
    m_numVertices = vertexCount;
    m_numTriangles = triangleCount;
    m_numIndices = triangleCount * 3;
    m_positions.createBuffer(vertexCount);
    m_normals.createBuffer(vertexCount);
    m_indices.createBuffer(m_numIndices);
}

void ATriangleMesh::reverseTriangleNormals()
{
	const int& n = numTriangles();
    unsigned* triv = indices();
    
	for(int i=0;i<n;++i) {
		unsigned* v = &triv[i*3];
		unsigned t = v[0];
		v[0] = v[2];
		v[2] = t;
	}
}

void ATriangleMesh::calculateVertexNormals()
{
    const int& nv = numVertices();
    Vector3F* ndst = normals();
    for(int i=0;i<nv;++i) {
        ndst[i].set(0.f, 0.f, 0.f);
    }
    
    const int& n = numTriangles();
    const unsigned* triv = c_indices();
    for(int i=0;i<n;++i) {
        const Vector3F triNm = getTriangleNormal(i);
        const unsigned* triInd = &triv[i*3];
        ndst[triInd[0]] += triNm;
        ndst[triInd[1]] += triNm;
        ndst[triInd[2]] += triNm;
    }
    
    for(int i=0;i<nv;++i) {
        ndst[i].normalize();
    }
    
}

Vector3F ATriangleMesh::getTriangleNormal(int i) const
{
    const Vector3F* p = c_positions();
	const unsigned* v = &c_indices()[i*3];
	const Vector3F& a = p[v[0]];
	const Vector3F& b = p[v[1]];
	const Vector3F& c = p[v[2]];
	Vector3F ab = b - a;
	Vector3F ac = c - a;
	Vector3F nor = ab.cross(ac);
	nor.normalize();
    return nor;
}

/// https://en.wikipedia.org/wiki/Heron's_formula
float ATriangleMesh::getTriangleArea(int i) const
{
    const Vector3F* p = c_positions();
	const unsigned* v = &c_indices()[i*3];
    const Vector3F& a = p[v[0]];
	const Vector3F& b = p[v[1]];
	const Vector3F& c = p[v[2]];

	float la = (b - a).length();
	float lb = (c - b).length();
	float lc = (a - c).length();
    float s = 0.5f * (la + lb + lc);
	
    return sqrt(s * (s - la) * (s - lb) * (s - lc));
}

void ATriangleMesh::createPositionNormalArray(SimpleBuffer<Vector3F>& posnml) const
{
    const Vector3F* p = c_positions();
    const Vector3F* n = c_normals();
    posnml.createBuffer(m_numIndices * 2);
    for(int i=0;i<m_numTriangles;++i) {
        const unsigned* v = &c_indices()[i*3];
        const int i6 = i * 6;
        posnml[i6]   = p[v[0]];
        posnml[i6+1] = n[v[0]];
        posnml[i6+2] = p[v[1]];
        posnml[i6+3] = n[v[1]];
        posnml[i6+4] = p[v[2]];
        posnml[i6+5] = n[v[2]];
    }
}

void ATriangleMesh::createBarycentricCoordinates(SimpleBuffer<Vector3F>& baryc) const
{
    baryc.createBuffer(m_numIndices);
    const int n = baryc.capacity() / 3;
    for(int i=0;i<n;++i) {
        baryc[i*3].set(1.f, 0.f, 0.f);
        baryc[i*3+1].set(0.f, 1.f, 0.f);
        baryc[i*3+2].set(0.f, 0.f, 1.f);
    }
}

}
//:~