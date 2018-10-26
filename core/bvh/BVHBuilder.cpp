#include "BVHBuilder.h"
#include "BVH.h"
#include "BVHNode.h"
#include "BVHSplit.h"

namespace alo {

BVHBuilder::BVHBuilder()
{}

BVHBuilder::~BVHBuilder()
{}

void BVHBuilder::build(BVH *hierarchy)
{
	BVHSplit *split = new BVHSplit(0, hierarchy->nodes(), hierarchy->primitives() );
	m_buildQueue.push_back(split);

	int maxQLen = 1;

	for(;;) {

		BVHSplit *active = m_buildQueue.front();
		if(active->compute() ) {

			hierarchy->splitNode(active->nodeIndex());
			BVHNode *parent = &hierarchy->nodes()[active->nodeIndex()];
			const int ileft = parent->leftChild();

			BVHNode *lft = &hierarchy->nodes()[ileft];
			BVHNode *rgt = lft++;

			active->setChildren(lft, rgt);

			active->sortPrimitives();

			BVHSplit *lftSplit = new BVHSplit(ileft, hierarchy->nodes(), hierarchy->primitives() );
			m_buildQueue.push_back(lftSplit);

			BVHSplit *rgtSplit = new BVHSplit(ileft + 1, hierarchy->nodes(), hierarchy->primitives() );
			m_buildQueue.push_back(rgtSplit);

		}
		
		delete active;
		m_buildQueue.erase(m_buildQueue.begin());

		if(m_buildQueue.size() < 1) break;

		if(maxQLen < m_buildQueue.size())
			maxQLen = m_buildQueue.size();

	}
	std::cout<<" max queued splits "<<maxQLen;
}

}
