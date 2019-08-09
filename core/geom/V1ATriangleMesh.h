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
#include <math/NamedBufferArray.h>

namespace alo {

class Matrix33F;
       
namespace ver1 {

class ATriangleMesh {
	
    SimpleBuffer<Vector3F> m_positions;
    SimpleBuffer<Vector3F> m_normals;
    SimpleBuffer<Int3> m_indices;
/// name and face-varying texture coordinate
    NamedBufferArray<Float2, 16> m_uvSets;
    int m_numVertices;
    int m_numTriangles;
    int m_numIndices;
    
public:
	ATriangleMesh();
	virtual ~ATriangleMesh();

    void createTriangleMesh(int vertexCount, int triangleCount);
    void purgeMesh();

    void copyPositionsFrom(const Vector3F *x);
    void copyIndicesFrom(const Int3 *x);
    void createUVSets(int n);
    void setUVSetName(const std::string &name, int i);
    Float2 *addUVSet(const std::string &name);
    Float2 *uvSet(int i);
    void clearUVSets();

    const int& numVertices() const;
    const int& numTriangles() const;
    const int& numIndices() const;
    const int &numUVSets() const;
    
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

    void scaleBy(float x);
    void translateBy(float x, float y, float z);
    void rotateBy(const Matrix33F &mat);
/// nv vertices nf faces to b
    void copyMeshTo(ATriangleMesh *b, const int &nv, const int &nf) const;
    
    static float CalcArea(const Vector3F &a, const Vector3F &b, const Vector3F &c);
///     a
//      |
///     |
///  b --- c
/// perpendicular to bc pointing to a
    static Vector3F PerpendicularToOppositeEdge(const Vector3F &a, const Vector3F &b, const Vector3F &c);

    template<typename T, typename T1>
    void getTriangle(T1 & sampler, const int & i) const;

protected:
    void setNumVertices(int x);
    void setNumTriangles(int x);
    
    SimpleBuffer<Vector3F>& positionBuffer();
    SimpleBuffer<Vector3F>& normalBuffer();
    SimpleBuffer<Int3>& indexBuffer();
    SimpleBuffer<Float2> &uvBuffer(int i);
    
private:
    
};

template<typename T, typename T1>
void ATriangleMesh::getTriangle(T1 & sampler, const int & i) const
{
	const Int3 &t = c_indices()[i];
	memcpy(sampler.index(), (const int *)&t, 12);
    
	const Vector3F *p = c_positions();
    const Vector3F *n = c_normals();
	
    T &v0 = sampler.vertex()[0];
    v0._pos = p[t.x];
    v0._nml = n[t.x];
    
    T &v1 = sampler.vertex()[1];
    v1._pos = p[t.y];
    v1._nml = n[t.y];
    
    T &v2 = sampler.vertex()[2];
    v2._pos = p[t.z];
    v2._nml = n[t.z];
			
    if(numUVSets() < 1) {
        v0._uv[0] = v0._uv[1] = 0.f;
        v1._uv[0] = 1.f; v1._uv[1] = 0.f;
        v2._uv[0] = v2._uv[1] = 1.f;
    } else {
        const Float2 *uvs = c_uvSet(0);
        const Float2 *triUv = &uvs[i*3];
        memcpy(v0._uv, triUv, 8);
        memcpy(v1._uv, &triUv[1], 8);
        memcpy(v2._uv, &triUv[2], 8);
    }
}

}

}
#endif //  
