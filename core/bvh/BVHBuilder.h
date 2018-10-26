/*
 * BVHBuilder.h
 *
 * 
 */

#ifndef ALO_BVH_BUILDER_H
#define ALO_BVH_BUILDER_H

#include <deque>

namespace alo {

class BoundingBox;
class BVHSplit;
class BVHPrimitive;

class BVHBuilder {

	std::deque<BVHSplit *> m_buildQueue;

public:

	BVHBuilder();
	~BVHBuilder();

	void build(const BoundingBox &parentAABB, 
				BVHPrimitive *primtives, int begin, int end);

private:


};

}

#endif
