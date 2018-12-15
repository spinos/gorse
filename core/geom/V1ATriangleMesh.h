/*
 *  V1ATriangleMesh.h
 *  aloe
 *
 *  int3 as face indices
 *  uv is facing-varying
 *
 */

#ifndef ALO_GEOM_V1_ATRIANGLE_MESH_H
#define ALO_GEOM_V1_ATRIANGLE_MESH_H
 
#include <math/SimpleBuffer.h>
#include <math/BoundingBox.h>
#include <math/Float2.h>
#include <math/Int3.h>
#include <deque>

namespace alo {
       
namespace ver1 {

/// uv set name and face-varying texture coordinate
typedef std::pair<std::string, SimpleBuffer<Float2> > NamedUV;

class ATriangleMesh {
	
    SimpleBuffer<Vector3F> m_positions;
    SimpleBuffer<Vector3F> m_normals;
    SimpleBuffer<Int3> m_indices;
    int m_numVertices;
    int m_numTriangles;
    int m_numIndices;
    std::deque<NamedUV > m_uvSets;
    
public:
	ATriangleMesh();
	virtual ~ATriangleMesh();

    void createTriangleMesh(int vertexCount, int triangleCount);
    void purgeMesh();

    void copyPositionsFrom(const Vector3F *x);
    void copyIndicesFrom(const unsigned *x);
    void createUVSets(int n);
    void setUVSetName(const std::string &name, int i);
    Float2 *addUVSet(const std::string &name);
    Float2 *uvSetValue(int i);
    void clearUVSets();

    const int& numVertices() const;
    const int& numTriangles() const;
    const int& numIndices() const;
    int numUVSets() const;
    
    const Int3* c_indices() const;
    const Vector3F* c_positions() const;
    const Vector3F* c_normals() const;
    const Float2 *c_uvSet(const std::string &name) const;
/// i-th uv set
    const std::string &c_uvName(int i) const;
    const Float2 *c_uvSet(int i) const;
    
/// face-varying vertex attribute
    void createPositionNormalArray(SimpleBuffer<Vector3F>& posnml) const;
    void createUVNormalArray(SimpleBuffer<Vector3F>& posnml) const;

    void getAabb(BoundingBox &b) const;
    void reverseTriangleNormals();
    void calculateVertexNormals();
    Vector3F getTriangleNormalMultArea(int i) const;
    Vector3F getTriangleNormal(int i) const;
    float getTriangleArea(int i) const;
    void getTriangleAabb(BoundingBox &b, int i) const;
    bool replaceFaceVertex(int i, int a, int b);
/// a < b < c
    bool checkFaceVertex(int i, int a, int b, int c) const;
    void printFace(int i) const;

    Int3* indices();
    Vector3F* positions();
    Vector3F* normals();
    
protected:
    void setNumVertices(int x);
    void setNumTriangles(int x);
    
    SimpleBuffer<Vector3F>& positionBuffer();
    SimpleBuffer<Vector3F>& normalBuffer();
    SimpleBuffer<Int3>& indexBuffer();
    std::deque<NamedUV >::iterator uvBegin();
    std::deque<NamedUV >::iterator uvEnd();
    std::deque<NamedUV >::const_iterator c_uvBegin() const;
    std::deque<NamedUV >::const_iterator c_uvEnd() const;
    SimpleBuffer<Float2> &uvBuffer(int i);
    
private:
    
};

}

}
#endif //  
