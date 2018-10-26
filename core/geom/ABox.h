/*
 *  ABox.h
 *  
 *  24 vertices 12 triangles
 *
 *
 */

#ifndef ALO_A_BOX_H
#define ALO_A_BOX_H

#include "ATriangleMesh.h"

namespace alo {

class ABox : public ATriangleMesh {

	static const unsigned BoxFaceIndices[];
    static const float BoxVertexNormals[];
    
public:

	ABox();
	
	void updateBox(const BoundingBox &box);

protected:

private:
	
};

}

#endif
