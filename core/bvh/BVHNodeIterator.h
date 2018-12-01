/*
 *  BVHNodeIterator.h
 *  aloe
 *
 *
 */

#ifndef ALO_BVH_NODE_ITERATOR_H
#define ALO_BVH_NODE_ITERATOR_H

#include "BVHNode.h"

namespace alo {

struct BVHNodeIterator {
	const BVHNode *_node;
	int _ind;
};

}

#endif
