/*
 * BVHBuilder.h
 *
 * 
 */

#ifndef ALO_BVH_BUILDER_H
#define ALO_BVH_BUILDER_H

#include <deque>

namespace alo {

class BVH;
class BVHSplit;

class BVHBuilder {

	std::deque<BVHSplit *> m_buildQueue;

public:

	BVHBuilder();
	~BVHBuilder();

	void build(BVH *hierarchy);

private:


};

}

#endif
