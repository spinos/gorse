#include "BVHBuilder.h"
#include "BVH.h"
#include "BVHBuildTraverseRule.h"
#include "BVHSplit.h"

//#include <deque>

namespace alo {

void BVHBuilder::Build(BVH *hierarchy)
{
    
    bvh::BuildTraverseRule rule;
    rule.attach(hierarchy);
    
    bvh::BuildTraverseResult param;
    rule.begin(param);
    
    for(;;) {
        
        rule.traverse(param);
        
        if(rule.end(param) ) break;
        
        //param.printCurrent();
        
        BVHSplit split(param._current, hierarchy);
        param._toSplit = split.compute();
        if(param._toSplit) {
            const int ileft = hierarchy->splitNode(param._current);
            //std::cout << " split " << param._current << " " << ileft;
            
            BVHNode *lft = &hierarchy->node(ileft);
			BVHNode *rgt = lft++;
            
            split.setChildren(lft, rgt);

			split.sortPrimitives();
        }
    }
    
    hierarchy->buildIndirection();
    
    /*
	std::deque<BVHSplit *> buildQueue;

	BVHSplit *split = new BVHSplit(0, hierarchy );
	buildQueue.push_back(split);
	
	int maxQLen = 1;

	for(;;) {

		BVHSplit *active = buildQueue.front();
		if(active->compute() ) {

			hierarchy->splitNode(active->nodeIndex());
			const BVHNode &parent = hierarchy->c_nodes()[active->nodeIndex()];
			const int ileft = parent.leftChild();

			BVHNode *lft = &hierarchy->nodes()[ileft];
			BVHNode *rgt = lft++;

			active->setChildren(lft, rgt);

			active->sortPrimitives();

			BVHSplit *lftSplit = new BVHSplit(ileft, hierarchy );
			buildQueue.push_back(lftSplit);

			BVHSplit *rgtSplit = new BVHSplit(ileft + 1, hierarchy );
			buildQueue.push_back(rgtSplit);

		}
		
		delete active;
		buildQueue.erase(buildQueue.begin());

		if(buildQueue.size() < 1) break;

		if(maxQLen < buildQueue.size())
			maxQLen = buildQueue.size();

	}*/
    
	std::cout<<".";
}

}
