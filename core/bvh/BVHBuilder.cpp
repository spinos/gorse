#include "BVHBuilder.h"
#include "BVH.h"
#include "BVHNode.h"
#include "BVHSplit.h"
#include <deque>

namespace alo {

void BVHBuilder::Build(BVH *hierarchy)
{
	std::deque<BVHSplit *> buildQueue;

	BVHSplit *split = new BVHSplit(0, hierarchy->nodes(), hierarchy->primitives() );
	buildQueue.push_back(split);

	int maxQLen = 1;

	for(;;) {

		BVHSplit *active = buildQueue.front();
		if(active->compute() ) {

			hierarchy->splitNode(active->nodeIndex());
			BVHNode *parent = &hierarchy->nodes()[active->nodeIndex()];
			const int ileft = parent->leftChild();

			BVHNode *lft = &hierarchy->nodes()[ileft];
			BVHNode *rgt = lft++;

			active->setChildren(lft, rgt);

			active->sortPrimitives();

			BVHSplit *lftSplit = new BVHSplit(ileft, hierarchy->nodes(), hierarchy->primitives() );
			buildQueue.push_back(lftSplit);

			BVHSplit *rgtSplit = new BVHSplit(ileft + 1, hierarchy->nodes(), hierarchy->primitives() );
			buildQueue.push_back(rgtSplit);

		}
		
		delete active;
		buildQueue.erase(buildQueue.begin());

		if(buildQueue.size() < 1) break;

		if(maxQLen < buildQueue.size())
			maxQLen = buildQueue.size();

	}
	std::cout<<".";
}

}
