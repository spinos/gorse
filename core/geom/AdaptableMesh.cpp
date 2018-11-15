/*
 *  AdaptableMesh.cpp
 *  aloe
 *
 *  Created by jian zhang on 4/25/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include "AdaptableMesh.h"
#include <map>

namespace alo {

AdaptableMesh::AdaptableMesh() 
{ initAdaptableMesh(); }

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

void AdaptableMesh::swapVertex(int va, int vb,
                const std::vector<int> &facesa,
                const std::vector<int> &facesb)
{
    positionBuffer().swap(va, vb, 1);
    normalBuffer().swap(va, vb, 1);

    std::vector<int>::const_iterator it = facesa.begin();
    for(;it!=facesa.end();++it) {
        unsigned *v = &indices()[*it * 3];
        if(v[0] == va) v[0] = vb;
        if(v[1] == va) v[1] = vb;
        if(v[2] == va) v[2] = vb;
    }

    it = facesb.begin();
    for(;it!=facesb.end();++it) {
        unsigned *v = &indices()[*it * 3];
        if(v[0] == vb) v[0] = va;
        if(v[1] == vb) v[1] = va;
        if(v[2] == vb) v[2] = va;
    }
}

void AdaptableMesh::removeLastVertices(int x)
{ 
    int nv = numVertices() - x;
    setNumVertices(nv); 
}

void AdaptableMesh::removeLastFaces(int x)
{ 
    int nt = numTriangles() - x;
    setNumTriangles(nt); 
}

void AdaptableMesh::swapFace(int fromFace, int toFace)
{ indexBuffer().swap(fromFace * 3, toFace * 3, 3); }

void AdaptableMesh::swapFaceUV(int fromFace, int toFace)
{
    std::deque<NamedUV >::iterator it = uvBegin();
    for(;it!=uvEnd();++it) {
        it->second.swap(fromFace * 3, toFace * 3, 3);
    }
}

void AdaptableMesh::insertFaces(const std::vector<int> &faceVertices, int toFirstFace)
{
    const int nv = faceVertices.size();
    unsigned *b = new unsigned[nv];
    for(int i=0;i<nv;++i) {
        b[i] = faceVertices[i];
    }
    indexBuffer().insert(b, nv, toFirstFace * 3);
    delete[] b;

    const int nf = nv / 3;
    int nt = numTriangles() + nf;
    setNumTriangles(nt); 
}

void AdaptableMesh::appendFaces(const unsigned *faceVertices, int n)
{
    int nt = numTriangles() + n; 
    indexBuffer().createBuffer(nt);
    memcpy(&indices()[numTriangles() * 3], faceVertices, n * 12);
    setNumTriangles(nt);
}

void AdaptableMesh::setNumFaces(int n)
{ setNumTriangles(n); }

void AdaptableMesh::copyTo(AdaptableMesh *b, const int &nv, const int &nf) const
{
    b->createTriangleMesh(c_indices(), 
        c_positions(), c_normals(),
         nv, nf);
    b->clearUVSets();
    std::deque<NamedUV >::const_iterator it = c_uvBegin();
    for(;it!=c_uvEnd();++it) {
        Float2 *uvs = b->addUVSet(it->first);
        memcpy(uvs, it->second.c_data(), nf * 3 * 8);
    }
}

}
//:~