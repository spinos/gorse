#ifndef BVH_PRIMITIVE_H
#define BVH_PRIMITIVE_H

#include <math/BoundingBox.h>

namespace alo {

class BVHPrimitive {

	BoundingBox m_aabb;

public:

	BVHPrimitive();

	void setAABB(const BoundingBox &box);
	void setIndex(int x);

	const BoundingBox &aabb() const;
	const int &index() const;

};

}

#endif
