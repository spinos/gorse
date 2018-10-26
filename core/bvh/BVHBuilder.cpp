#include "BVHBuilder.h"
#include "BVHSplit.h"

namespace alo {

BVHBuilder::BVHBuilder()
{}

BVHBuilder::~BVHBuilder()
{}

void BVHBuilder::build(const BoundingBox &parentAABB, 
				BVHPrimitive *primtives, int begin, int end)
{
	BVHSplit *split = new BVHSplit(parentAABB, primtives, begin, end);
	m_buildQueue.push_back(split);

	while(m_buildQueue.size() > 0) {

		std::deque<BVHSplit *>::iterator it = m_buildQueue.begin();
		BVHSplit *active = *it;
		active->compute();

		delete active;
		m_buildQueue.erase(it);

	}
}

}
