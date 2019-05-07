#include "BVHBuilder.h"
#include "BVH.h"
#include "BVHBuildTraverseRule.h"
#include "BVHSplit.h"
#include "Binning.h"

namespace alo {

void BVHBuilder::Build(BVH *hierarchy)
{
    Binning bins[3];
    
    bvh::BuildTraverseRule rule;
    rule.attach(hierarchy);
    
    bvh::BuildTraverseResult param;
    rule.begin(param);
    
    for(;;) {
        
        rule.traverse(param);
        
        if(rule.end(param) ) break;
        
        BVHSplit split(param._current, hierarchy);
        param._toSplit = split.compute(bins);
        if(param._toSplit) {
            const int ileft = hierarchy->splitNode(param._current);
            
            BVHNode *lft = &hierarchy->node(ileft);
			BVHNode *rgt = &hierarchy->node(ileft + 1);

            split.setChildren(lft, rgt, bins);

			split.sortPrimitives(bins);
            
        }
    }
    
    hierarchy->buildIndirection();
    
	std::cout<<".";
}

}
