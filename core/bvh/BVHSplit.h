/*
 *  BVHSplit.h
 *
 *  2019/5/7
 */

#ifndef BVH_SPLIT_H
#define BVH_SPLIT_H

#include <math/BoundingBox.h>

namespace alo {

class BVH;
class BVHNode;
class Binning;

class BVHSplit {

	BVH *m_hierarchy;
	BoundingBox m_aabb;
	int m_nodeIndex;
	int m_begin;
    int m_end;
	float m_splitCost;
	int m_splitDim;
	int m_splitInd;
    
public:
	BVHSplit(int ind, BVH *hierarchy);
	~BVHSplit();

/// 3 bins
	bool compute(Binning *bins);
	void sortPrimitives(const Binning *bins);
	void setChildren(BVHNode *leftChild, BVHNode *rightChild, const Binning *bins);

	const int &nodeIndex() const;
	float getNonSplitCost() const;
	const float &splitCost() const;
	const int &splitAxis() const;
	const float &splitPos(const Binning *bins) const;
	const BoundingBox &splitLeftBox(const Binning *bins) const;
	const BoundingBox &splitRightBox(const Binning *bins) const;
	const int &begin() const;
	const int &end() const;

	static int InnerNumPrimitives;
	static int LeafNumPrimitives;

	friend std::ostream& operator<<(std::ostream &output, const BVHSplit & p);

private:
	void splitDim(int dim, float dx, Binning &bi);
	float computeLowestCostSplit(Binning *bins);
	float computeCost(const int &nl, 
			const int &nr,
			const BoundingBox &bl,
			const BoundingBox &br,
			const float &abp) const;

};

}

#endif
