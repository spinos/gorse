/*
 *  Hexahedron.h
 *  aloe
 *
 *  quadrilaterally-faced
 *  8 points 6 faces 12 edges
 *      
 *    2 - 3
 *    |   |
 *    0 - 1
 *  6 - 7 
 *  |   | 
 *  4 - 5
 *
 */

#ifndef ALO_HEXAHEDRON_H
#define ALO_HEXAHEDRON_H

#include "Vector3F.h"

namespace alo {

class BoundingBox;
	
class Hexahedron {

	Vector3F m_p[8];

public:
	Hexahedron();
	void set(const BoundingBox &aabb);

	float size() const;

	Vector3F supportPoint(const Vector3F & v, Vector3F * localP = 0) const;

	int numPoints() const;
	const Vector3F &X(int i) const;
	
};

}

#endif
