/*
 *  AdaptableMesh.cpp
 *  aloe
 *
 *  Created by jian zhang on 4/25/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include "AdaptableMesh.h"

namespace alo {

AdaptableMesh::AdaptableMesh() 
{}

AdaptableMesh::~AdaptableMesh() 
{}

void AdaptableMesh::initAdaptableMesh()
{
    createTriangleMesh(1000, 2000);
    setNumVertices(0);
    setNumTriangles(0);
}

int AdaptableMesh::addVertex(const Vector3F& pos)
{
    const int nv = numVertices();
    positionBuffer().createBuffer(nv + 1);
    normalBuffer().createBuffer(nv + 1);
    setNumVertices(nv + 1);
    
    positionBuffer()[nv] = pos;
    return nv;
}

void AdaptableMesh::addTriangle(int v0, int v1, int v2)
{
    const int nt = numTriangles();
    indexBuffer().createBuffer((nt + 1)* 3);
    setNumTriangles(nt + 1);
    
    indexBuffer()[nt * 3] = v0;
    indexBuffer()[nt * 3 + 1] = v1;
    indexBuffer()[nt * 3 + 2] = v2;
}

void AdaptableMesh::addQuad(int v0, int v1, int v2, int v3,
                            bool flipped)
{
    const int nt = numTriangles();
    indexBuffer().createBuffer((nt + 2)* 3);
    setNumTriangles(nt + 2);
    
    indexBuffer()[nt * 3] = v0;
        
    if(flipped) {
        indexBuffer()[nt * 3 + 1] = v1;
        indexBuffer()[nt * 3 + 2] = v3;
        indexBuffer()[nt * 3 + 3] = v1;
        indexBuffer()[nt * 3 + 4] = v2;
    } else {
        indexBuffer()[nt * 3 + 1] = v1;
        indexBuffer()[nt * 3 + 2] = v2;
        indexBuffer()[nt * 3 + 3] = v0;
        indexBuffer()[nt * 3 + 4] = v2;
    }
    
    indexBuffer()[nt * 3 + 5] = v3;
}

Vector3F* AdaptableMesh::vertexPositionR(int i)
{ return &positions()[i]; }

}
//:~