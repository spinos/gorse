/*
 *  SparseVoxelNtree.h
 *  aloe
 *  
 *  N is level added after each subdivision
 *  (1<<N)^3 sub nodes , when N = 1 is octree
 *
 *  T is node type Ts sample type
 *  build sparse voxel n-tree from samples
 *
 *  Created by jian zhang on 2/13/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SPARSE_VOXEL_N_TREE_H
#define ALO_SPARSE_VOXEL_N_TREE_H

#include "SvtBuilder.h"
#include "LegendreBuildRule.h"
#include "SvtTraverser.h"
#include "SvtTraverseContext.h"
#include "LegendreTraverseRule.h"

namespace alo {

namespace sds {

/// for building 
struct VoxelNode {

/// sfc
	int _key;
/// end of sfc	
	int _nextKey;
/// range of samples
	int _range[2];
	int _level;
	int _ind;
    int _dataInd;
/// build box (center, half_sapn)
	float _coord[4];
	
};

/// for traversing
struct SvtNode {

    int _key;
	
};

}

}

#endif
