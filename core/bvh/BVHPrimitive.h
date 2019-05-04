#ifndef BVH_PRIMITIVE_H
#define BVH_PRIMITIVE_H

#include <math/BoundingBox.h>

namespace alo {

class BVHPrimitive {

	BoundingBox m_aabb;

public:

	BVHPrimitive();

	void setBBox(const BoundingBox &box);
	void setIndex(int x);
	void setKey(int x);

	const BoundingBox &bbox() const;
	const int &index() const;
	const int &key() const;

};

}

#endif
