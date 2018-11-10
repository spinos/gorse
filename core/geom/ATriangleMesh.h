/*
 *  ATriangleMesh.h
 *  aloe
 *
 *  Created by jian zhang on 4/25/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_GEOM_TRIANGLE_MESH_H
#define ALO_GEOM_TRIANGLE_MESH_H
 
#include <math/SimpleBuffer.h>
#include <math/BoundingBox.h>

namespace alo {

class ATriangleMesh {
	
    SimpleBuffer<Vector3F> m_positions;
    SimpleBuffer<Vector3F> m_normals;
    SimpleBuffer<unsigned> m_indices;
    int m_numVertices;
    int m_numTriangles;
    int m_numIndices;
    
public:
	ATriangleMesh();
	virtual ~ATriangleMesh();

    void createTriangleMesh(int vertexCount, int triangleCount);
    void createTriangleMesh(const unsigned *inds,
        const Vector3F *pos, const Vector3F *nml,
        int vertexCount, int triangleCount);
    
    void copyPositionsFrom(const Vector3F *x);
    void copyIndicesFrom(const unsigned *x);
    
    const int& numVertices() const;
    const int& numTriangles() const;
    const int& numIndices() const;
    
    const unsigned* c_indices() const;
    const Vector3F* c_positions() const;
    const Vector3F* c_normals() const;
    
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
    
private:
    
};



}
#endif //  #ifndef APH_GEOM_TRIANGLE_MESH_H