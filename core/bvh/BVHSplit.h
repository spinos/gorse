#ifndef BVH_SPLIT_H
#define BVH_SPLIT_H

#include "Binning.h"
#include "BVHPrimitive.h"

namespace alo {

class BVHSplit {

	BoundingBox m_aabb;
	BVHPrimitive *m_primitives;
	int m_begin, m_end;
	Binning m_bins[3];
	int m_splitDim;
	int m_splitInd;
	
public:
	BVHSplit(const BoundingBox &parentAABB, 
				BVHPrimitive *primtives, int begin, int end);

	bool compute();

private:
	void splitDim(int dim, float dx);
	float computeLowestCostSplit();
	float computeCost(const int &nl, 
			const int &nr,
			const BoundingBox &bl,
			const BoundingBox &br,
			const float &abp) const;

};

}

#endif
