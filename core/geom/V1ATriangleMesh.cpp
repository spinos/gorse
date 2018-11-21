/*
 *  ATriangleMesh.cpp
 *  aloe
 *
 *
 */

#include "V1ATriangleMesh.h"

namespace alo {
    
namespace ver1 {

ATriangleMesh::ATriangleMesh() : m_numVertices(0),
    m_numTriangles(0),
    m_numIndices(0)
{}

ATriangleMesh::~ATriangleMesh() 
{ m_uvSets.clear(); }

const int& ATriangleMesh::numVertices() const
{ return m_numVertices; }

const int& ATriangleMesh::numTriangles() const
{ return m_numTriangles; }

const int& ATriangleMesh::numIndices() const
{ return m_numIndices; }

int ATriangleMesh::numUVSets() const
{ return m_uvSets.size(); }

const Vector3F* ATriangleMesh::c_positions() const
{ return m_positions.c_data(); }

const Vector3F* ATriangleMesh::c_normals() const
{ return m_normals.c_data(); }

const Int3* ATriangleMesh::c_indices() const
{ return m_indices.c_data(); }

Int3* ATriangleMesh::indices()
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

SimpleBuffer<Int3>& ATriangleMesh::indexBuffer()
{ return m_indices; }

void ATriangleMesh::purgeMesh()
{
    m_positions.purgeBuffer();
    m_normals.purgeBuffer();
    m_indices.purgeBuffer();
    m_numVertices = m_numTriangles = m_numIndices = 0;
    clearUVSets();
}

void ATriangleMesh::createTriangleMesh(int vertexCount, int triangleCount)
{
    m_numVertices = vertexCount;
    m_numTriangles = triangleCount;
    m_numIndices = triangleCount * 3;
    m_positions.createBuffer(vertexCount);
    m_normals.createBuffer(vertexCount);
    m_indices.createBuffer(m_numTriangles);
}

void ATriangleMesh::createTriangleMesh(const unsigned *inds,
        const Vector3F *pos, const Vector3F *nml,
        int vertexCount, int triangleCount)
{
    createTriangleMesh(vertexCount, triangleCount);
    m_indices.copyFrom((const Int3 *)inds, triangleCount);
    m_positions.copyFrom(pos, vertexCount);
    m_normals.copyFrom(nml, vertexCount);
}

void ATriangleMesh::copyPositionsFrom(const Vector3F *x)
{ m_positions.copyFrom(x, m_numVertices); }

void ATriangleMesh::copyIndicesFrom(const unsigned *x)
{ m_indices.copyFrom((const Int3 *)x, m_numTriangles); }

Float2 *ATriangleMesh::addUVSet(const std::string &name)
{
    m_uvSets.push_front(NamedUV());
    NamedUV &fuv = m_uvSets.front();
    fuv.first = name;
    fuv.second.createBuffer(m_numIndices);
    return fuv.second.data();
}

void ATriangleMesh::clearUVSets()
{ m_uvSets.clear(); }

const Float2 *ATriangleMesh::c_uvSet(const std::string &name) const
{
    std::deque<NamedUV >::const_iterator it = m_uvSets.begin();
    for(;it!=m_uvSets.end();++it) {
        const NamedUV &auv = *it;
        if(auv.first == name) 
            return auv.second.c_data();
    }
    return 0;
}

const std::string &ATriangleMesh::c_uvName(int i) const
{
    const NamedUV &auv = m_uvSets[i];
    return auv.first;
}

const Float2 *ATriangleMesh::c_uvSet(int i) const
{
    const NamedUV &auv = m_uvSets[i];
    return auv.second.c_data();
}

void ATriangleMesh::reverseTriangleNormals()
{
	const int& n = numTriangles();
    Int3* triv = indices();
    
	for(int i=0;i<n;++i) {
		Int3 &fv = triv[i];
		int t = fv.x;
		fv.x = fv.z;
		fv.z = t;
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
    const Int3* triv = c_indices();
    for(int i=0;i<n;++i) {
        const Vector3F triNm = getTriangleNormalMultArea(i);
        const Int3 &fv = triv[i];
        ndst[fv.x] += triNm;
        ndst[fv.y] += triNm;
        ndst[fv.z] += triNm;
    }
    
    for(int i=0;i<nv;++i) {
        ndst[i].normalize();
    }
    
}

Vector3F ATriangleMesh::getTriangleNormalMultArea(int i) const
{
    const Vector3F* p = c_positions();
    const Int3 &fv = c_indices()[i];
    const Vector3F& a = p[fv.x];
    const Vector3F& b = p[fv.y];
    const Vector3F& c = p[fv.z];
    Vector3F ab = b - a;
    Vector3F ac = c - a;
    Vector3F nor = ab.cross(ac);
    nor.normalize();

    float la = (b - a).length();
    float lb = (c - b).length();
    float lc = (a - c).length();
    float s = 0.5f * (la + lb + lc);

    return nor * sqrt(s * (s - la) * (s - lb) * (s - lc));
}

Vector3F ATriangleMesh::getTriangleNormal(int i) const
{
    const Vector3F* p = c_positions();
	const Int3 &fv = c_indices()[i];
	const Vector3F& a = p[fv.x];
    const Vector3F& b = p[fv.y];
    const Vector3F& c = p[fv.z];
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
	const Int3 &fv = c_indices()[i];
    const Vector3F& a = p[fv.x];
    const Vector3F& b = p[fv.y];
    const Vector3F& c = p[fv.z];
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
        const Int3 &fv = c_indices()[i];
        const int i6 = i * 6;
        posnml[i6]   = p[fv.x];
        posnml[i6+1] = n[fv.x];
        posnml[i6+2] = p[fv.y];
        posnml[i6+3] = n[fv.y];
        posnml[i6+4] = p[fv.z];
        posnml[i6+5] = n[fv.z];
    }
}

void ATriangleMesh::createUVNormalArray(SimpleBuffer<Vector3F>& posnml) const
{
    const Float2 *uvs = c_uvSet(0);
    posnml.createBuffer(m_numIndices * 2);
    for(int i=0;i<m_numTriangles;++i) {
        const Float2 &uv0 = uvs[i*3];
        const Float2 &uv1 = uvs[i*3 + 1];
        const Float2 &uv2 = uvs[i*3 + 2];
        const int i6 = i * 6;
        posnml[i6  ].set(uv0.x, uv0.y, 0.f);
        posnml[i6+1] = Vector3F::ZAxis;
        posnml[i6+2].set(uv1.x, uv1.y, 0.f);
        posnml[i6+3] = Vector3F::ZAxis;
        posnml[i6+4].set(uv2.x, uv2.y, 0.f);
        posnml[i6+5] = Vector3F::ZAxis;
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

bool ATriangleMesh::replaceFaceVertex(int i, int a, int b)
{
    Int3 &fv = indices()[i];
    if(fv.x != a && fv.y != a && fv.z != a) {
        std::cout<<"\n WARNING ATriangleMesh replaceFaceVertex face "<<i
                <<" ("<<fv.x<<","<<fv.y<<","<<fv.z<<") "
                <<" cannot replace "<<a<<" to "<<b;
        return false;
    }
    if(fv.x == a) fv.x = b;
    if(fv.y == a) fv.y = b;
    if(fv.z == a) fv.z = b;
    return true;
}

bool ATriangleMesh::checkFaceVertex(int i, int a, int b, int c) const
{
    const Int3 &fv = c_indices()[i];

    int flo = fv.x;
    if(flo > fv.y) flo = fv.y;
    if(flo > fv.z) flo = fv.z;

    if(a != flo) return false;

    int fhi = fv.x;
    if(fhi < fv.y) fhi = fv.y;
    if(fhi < fv.z) fhi = fv.z;

    if(c != fhi) return false;

    int fmd = fv.x;
    if(fv.y > flo && fv.y < fhi) fmd = fv.y;
    if(fv.z > flo && fv.z < fhi) fmd = fv.z;

    return b == fmd;
}

void ATriangleMesh::printFace(int i) const
{
    const Int3 &fv = c_indices()[i];
    std::cout << " face "<<i<<" (" << fv.x << "," << fv.y << "," << fv.z << ") ";               
}

std::deque<NamedUV >::iterator ATriangleMesh::uvBegin()
{ return m_uvSets.begin(); }

std::deque<NamedUV >::iterator ATriangleMesh::uvEnd()
{ return m_uvSets.end(); }

std::deque<NamedUV >::const_iterator ATriangleMesh::c_uvBegin() const
{ return m_uvSets.begin(); }

std::deque<NamedUV >::const_iterator ATriangleMesh::c_uvEnd() const
{ return m_uvSets.end(); }

SimpleBuffer<Float2> &ATriangleMesh::uvBuffer(int i)
{ return m_uvSets[i].second; }

}

}
