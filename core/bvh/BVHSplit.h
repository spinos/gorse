/*
 *  BVHSplit.h
 *
 *  2019/5/7
 */

#ifndef BVH_SPLIT_H
#define BVH_SPLIT_H

#include "Binning.h"

namespace alo {

class BVH;
class BVHNode;

class BVHSplit {

	BoundingBox m_aabb;
	Binning m_bins[3];
	BVH *m_hierarchy;
	int m_nodeIndex;
	int m_begin, m_end;
	float m_splitCost;
	int m_splitDim;
	int m_splitInd;
	
public:
	BVHSplit(int ind, BVH *hierarchy);
	~BVHSplit();

	bool compute();
	void sortPrimitives();
	void setChildren(BVHNode *leftChild, BVHNode *rightChild);

	const int &nodeIndex() const;
	float getNonSplitCost() const;
	const float &splitCost() const;
	const int &splitAxis() const;
	const float &splitPos() const;
	const BoundingBox &splitLeftBox() const;
	const BoundingBox &splitRightBox() const;
	const int &begin() const;
	const int &end() const;

	static int InnerNumPrimitives;
	static int LeafNumPrimitives;

	friend std::ostream& operator<<(std::ostream &output, const BVHSplit & p);

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
