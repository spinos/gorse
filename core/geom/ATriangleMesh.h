#ifndef ALO_GEOM_TRIANGLE_MESH_H
#define ALO_GEOM_TRIANGLE_MESH_H

/*
 *  ATriangleMesh.h
 *  aloe
 *
 *  Created by jian zhang on 4/25/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */
 
#include <math/SimpleBuffer.h>
#include <math/Vector3F.h>

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
	
    const int& numVertices() const;
    const int& numTriangles() const;
    const int& numIndices() const;
    
    const unsigned* c_indices() const;
    const Vector3F* c_positions() const;
    const Vector3F* c_normals() const;
    
/// face-varying vertex
    void createPositionArray(SimpleBuffer<Vector3F>& pos) const;
    void createNormalArray(SimpleBuffer<Vector3F>& nml) const;
    void createBarycentricCoordinates(SimpleBuffer<Vector3F>& baryc) const;
    
protected:
	
    void createTriangleMesh(int vertexCount, int triangleCount);
	
	void reverseTriangleNormals();
    void calculateVertexNormals();
    Vector3F getTriangleNormal(int i) const;
    float getTriangleArea(int i) const;
	
    unsigned* indices();
    Vector3F* positions();
    Vector3F* normals();
    
private:
    
};



}
#endif //  #ifndef APH_GEOM_TRIANGLE_MESH_H