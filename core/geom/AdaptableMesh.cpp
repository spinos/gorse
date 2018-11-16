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
    indexBuffer().createBuffer(nt + 1);
    setNumTriangles(nt + 1);
    
    indexBuffer()[nt].set(v0, v1, v2);
}

void AdaptableMesh::addQuad(int v0, int v1, int v2, int v3,
                            bool flipped)
{
    const int nt = numTriangles();
    indexBuffer().createBuffer(nt + 2);
    setNumTriangles(nt + 2);
    
    Int3 &fv0 = indexBuffer()[nt];
    Int3 &fv1 = indexBuffer()[nt + 1];
    
        
    if(flipped) {
        fv0.set(v0, v1, v3);
        fv1.set(v1, v2, v3);
    } else {
        fv0.set(v0, v1, v2);
        fv1.set(v0, v2, v3);
    }
    
    
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
        Int3 &fv = indices()[*it];
        if(fv.x == va) fv.x = vb;
        if(fv.y == va) fv.y = vb;
        if(fv.z == va) fv.z = vb;
    }

    it = facesb.begin();
    for(;it!=facesb.end();++it) {
        Int3 &fv = indices()[*it];
        if(fv.x == vb) fv.x = va;
        if(fv.y == vb) fv.y = va;
        if(fv.z == vb) fv.z = va;
    }
}

void AdaptableMesh::removeLastVertices(int x)
{ 
    int nv = numVertices() - x;
    setNumVertices(nv); 
}

void AdaptableMesh::removeLastFaces(int x)
{ 
    const int nt = numTriangles() - x;
    setNumTriangles(nt); 
}

void AdaptableMesh::swapFace(int fromFace, int toFace)
{ indexBuffer().swap(fromFace, toFace, 1); }

void AdaptableMesh::swapFaceUV(int fromFace, int toFace)
{
    std::deque<ver1::NamedUV >::iterator it = uvBegin();
    for(;it!=uvEnd();++it) {
        it->second.swap(fromFace * 3, toFace * 3, 3);
    }
}

void AdaptableMesh::insertFaces(const std::vector<int> &faceVertices, int toFirstFace)
{
    const int nv = faceVertices.size();
    const int nf = nv / 3;
    Int3 *b = new Int3[nf];
    for(int i=0;i<nf;++i)
        b[i].set(faceVertices[i*3], faceVertices[i*3 + 1], faceVertices[i*3 + 2]);
    
    indexBuffer().insert(b, nf, toFirstFace);
    delete[] b;

    int nt = numTriangles() + nf;
    setNumTriangles(nt); 
}

void AdaptableMesh::appendFaces(const Int3 *faceVertices, int n)
{
    int nt = numTriangles() + n; 
    indexBuffer().createBuffer(nt);
    memcpy(&indices()[numTriangles()], faceVertices, n * 12);
    setNumTriangles(nt);
}

void AdaptableMesh::appendFaceUVs(const Float2 *faceUVs, int i, int n)
{
    SimpleBuffer<Float2> &uvi = uvBuffer(i);
    int nt = numTriangles() + n;
    uvi.createBuffer(nt * 3);
    memcpy(&uvi.data()[numTriangles() * 3], faceUVs, n * 24);
}

void AdaptableMesh::setNumFaces(int n)
{ setNumTriangles(n); }

void AdaptableMesh::copyTo(AdaptableMesh *b, const int &nv, const int &nf) const
{
    b->createTriangleMesh((const unsigned *)c_indices(), 
        c_positions(), c_normals(),
         nv, nf);
    b->clearUVSets();
    std::deque<ver1::NamedUV >::const_iterator it = c_uvBegin();
    for(;it!=c_uvEnd();++it) {
        Float2 *uvs = b->addUVSet(it->first);
        memcpy(uvs, it->second.c_data(), nf * 3 * 8);
    }
}

}
//:~