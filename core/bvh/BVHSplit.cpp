#include "BVHSplit.h"

namespace alo {

BVHSplit::BVHSplit(const BoundingBox &parentAABB, 
				BVHPrimitive *primtives, int begin, int end)
{ 
	m_aabb = parentAABB;
	m_primitives = primtives;
	m_begin = begin;
	m_end = end;
}

bool BVHSplit::compute()
{
	const float dl = m_aabb.getLongestDistance() / 32.f;

	splitDim(0, dl);
	splitDim(1, dl);
	splitDim(2, dl);

	float costSplit = computeLowestCostSplit();

	return costSplit < (m_end - m_begin);
}

void BVHSplit::splitDim(int dim, float dx)
{
	float b = m_aabb.getMax(dim);
	float a = m_aabb.getMin(dim);
	int n = (b - a) / dx;
	Binning &bi = m_bins[dim];
	if(n < 3) {
		bi.setEmpty();
		return;
	}

	bi.create(n-1);
	for(int i=0;i<n-1;++i)
		bi.setSplitPos(a + dx * (i+1), i);

	for(int i=m_begin;i<m_end;++i) {
		const BoundingBox &box = m_primitives[i].aabb();
		bi.count(box.getMin(dim), box);
	}

	bi.scan();

}

float BVHSplit::computeLowestCostSplit()
{ 
	const float a = m_aabb.area();
	float r = 1e28f;
	for(int d = 0;d<3;++d) {
		Binning &bi = m_bins[d];
		if(bi.isEmpty())
			continue;
		for(int i=0;i<bi.numSplits();++i) {
			float c = computeCost(bi.countLeft(i),
								bi.countRight(i),
								bi.aabbLeft(i),
								bi.aabbRight(i),
								a);
			if(c < r) {
				r = c;
				m_splitDim = d;
				m_splitInd = i;
			}
		}
	} 
	return r; 
}

float BVHSplit::computeCost(const int &nl, 
			const int &nr,
			const BoundingBox &bl,
			const BoundingBox &br,
			const float &abp) const
{ 
/// must contain primitive(s)
	if(nl < 1 || nr < 1) 
		return 1e28f;
	return 1.f + bl.area() / abp * nl + br.area() / abp * nr;
}

}
