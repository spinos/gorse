/*
 *  AdaptableMesh.h
 *  aloe
 *
 *  Created by jian zhang on 4/25/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_GEOM_ADAPTABLE_MESH_H
#define ALO_GEOM_ADAPTABLE_MESH_H

#include "ATriangleMesh.h"
#include <vector>

namespace alo {

class AdaptableMesh : public ATriangleMesh {

public:
	AdaptableMesh();
	virtual ~AdaptableMesh();
    
    void initAdaptableMesh();
    
    int addVertex(const Vector3F& pos);
    void addTriangle(int v0, int v1, int v2);

/// v3 - v2
/// |  / |
/// v0 - v1
/// or flipped
/// v3 - v2
/// |  \ |
/// v0 - v1
    void addQuad(int v0, int v1, int v2, int v3,
                bool flipped = false);
    
    Vector3F* vertexPositionR(int i);
/// swap vertex position and normal
/// updated connected faces to both
    void swapVertex(int va, int vb,
    			const std::vector<int> &facesa,
    			const std::vector<int> &facesb);
    void removeLastVertices(int x);
    void removeLastFaces(int x);
    void swapFace(int fromFace, int toFace);
    void insertFaces(const std::vector<int> &faceVertices, int toFirstFace);
 
protected:
    
private:
    
};



}
#endif //  #ifndef APH_GEOM_TRIANGLE_MESH_H