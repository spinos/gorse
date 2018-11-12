/*
 *  ATriangleMesh.h
 *  aloe
 *
 *  uv is facing-varying
 *
 */

#ifndef ALO_GEOM_TRIANGLE_MESH_H
#define ALO_GEOM_TRIANGLE_MESH_H
 
#include <math/SimpleBuffer.h>
#include <math/BoundingBox.h>
#include <math/Float2.h>
#include <deque>

namespace alo {

typedef std::pair<std::string, SimpleBuffer<Float2> > NamedUV;

class ATriangleMesh {
	
    SimpleBuffer<Vector3F> m_positions;
    SimpleBuffer<Vector3F> m_normals;
    SimpleBuffer<unsigned> m_indices;
    int m_numVertices;
    int m_numTriangles;
    int m_numIndices;
    std::deque<NamedUV > m_uvSets;
    
public:
	ATriangleMesh();
	virtual ~ATriangleMesh();

    void createTriangleMesh(int vertexCount, int triangleCount);
    void createTriangleMesh(const unsigned *inds,
        const Vector3F *pos, const Vector3F *nml,
        int vertexCount, int triangleCount);
    
    void copyPositionsFrom(const Vector3F *x);
    void copyIndicesFrom(const unsigned *x);
    Float2 *addUVSet(const std::string &name);

    const int& numVertices() const;
    const int& numTriangles() const;
    const int& numIndices() const;
    int numUVSets() const;
    
    const unsigned* c_indices() const;
    const Vector3F* c_positions() const;
    const Vector3F* c_normals() const;
    const Float2 *c_uvSet(const std::string &name) const;
    const Float2 *c_uvSet(int i) const;
    
/// face-varying vertex attribute
    void createPositionNormalArray(SimpleBuffer<Vector3F>& posnml) const;
    void createBarycentricCoordinates(SimpleBuffer<Vector3F>& baryc) const;
    
    void reverseTriangleNormals();
    void calculateVertexNormals();
    Vector3F getTriangleNormalMultArea(int i) const;
    Vector3F getTriangleNormal(int i) const;
    float getTriangleArea(int i) const;
    bool replaceFaceVertex(int i, int a, int b);
/// a < b < c
    bool checkFaceVertex(int i, int a, int b, int c) const;
    void printFace(int i) const;
    
protected:
    unsigned* indices();
    Vector3F* positions();
    Vector3F* normals();
    
    void setNumVertices(int x);
    void setNumTriangles(int x);
    
    SimpleBuffer<Vector3F>& positionBuffer();
    SimpleBuffer<Vector3F>& normalBuffer();
    SimpleBuffer<unsigned>& indexBuffer();
    std::deque<NamedUV >::iterator uvBegin();
    std::deque<NamedUV >::iterator uvEnd();
    
private:
    
};



}
#endif //  #ifndef APH_GEOM_TRIANGLE_MESH_H