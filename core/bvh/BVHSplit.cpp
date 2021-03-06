#include "BVHSplit.h"
#include "BVH.h"
#include "BVHNode.h"
#include "Binning.h"
#include <math/QuickSort.h>

namespace alo {

int BVHSplit::InnerNumPrimitives = 64;
int BVHSplit::LeafNumPrimitives = 16;

BVHSplit::BVHSplit(int ind, BVH *hierarchy)
{ 
	m_nodeIndex = ind;
	m_hierarchy = hierarchy;
	const BVHNode &parentNode = hierarchy->c_node(ind);
	m_aabb = parentNode.aabb();
	m_begin = parentNode.leafBegin();
	m_end = parentNode.leafEnd();
}

BVHSplit::~BVHSplit()
{}

const int &BVHSplit::nodeIndex() const
{ return m_nodeIndex; }

bool BVHSplit::compute(Binning *bins)
{
	if(m_end < m_begin + LeafNumPrimitives) return false;
    
	int n = m_end - m_begin;
	if(n > 32) n = 32;
	const float dl = m_aabb.getLongestDistance() / (float)n;
    
	splitDim(0, dl, bins[0]);
	splitDim(1, dl, bins[1]);
	splitDim(2, dl, bins[2]);
    
	m_splitCost = computeLowestCostSplit(bins);

	if(m_end > m_begin + InnerNumPrimitives) return true;
	return m_splitCost < getNonSplitCost();
}

void BVHSplit::splitDim(int dim, float dx, Binning &bi)
{
	float b = m_aabb.getMax(dim);
	float a = m_aabb.getMin(dim);
	int n = (b - a) / dx;

	if(n < 5) {
		bi.setEmpty();
		return;
	}
    
	bi.create(n-1);
	for(int i=0;i<n-1;++i)
		bi.setSplitPos(a + dx * (i+1), i);

	for(int i=m_begin;i<m_end;++i) {
		const BoundingBox &box = m_hierarchy->c_primitives()[i].bbox();
		bi.count(box.getMin(dim), box);
	}
    
	bi.scan();
    
}

float BVHSplit::computeLowestCostSplit(Binning *bins)
{ 
	const float a = m_aabb.area();
	float r = 1e28f;
	for(int d = 0;d<3;++d) {
		Binning &bi = bins[d];
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

float BVHSplit::getNonSplitCost() const
{ return (float)(m_end - m_begin); }

const float &BVHSplit::splitCost() const
{ return m_splitCost; }

const int &BVHSplit::splitAxis() const
{ return m_splitDim; }

const float &BVHSplit::splitPos(const Binning *bins) const
{ return bins[m_splitDim].splitPos(m_splitInd); }

const BoundingBox &BVHSplit::splitLeftBox(const Binning *bins) const
{ return bins[m_splitDim].aabbLeft(m_splitInd); }

const BoundingBox &BVHSplit::splitRightBox(const Binning *bins) const
{ return bins[m_splitDim].aabbRight(m_splitInd); }

void BVHSplit::sortPrimitives(const Binning *bins)
{
	const float thre = splitPos(bins);

	for(int i=m_begin;i<m_end;++i) {
		const BoundingBox &box = m_hierarchy->c_primitive(i).bbox();
		if(box.getMin(m_splitDim) < thre) 
			m_hierarchy->primitive(i).setKey(0);
		else
			m_hierarchy->primitive(i).setKey(1);
	}

	QuickSort1::SortByKeyF<int, BVHPrimitive >(m_hierarchy->primitives(), m_begin, m_end - 1);
}

void BVHSplit::setChildren(BVHNode *leftChild, BVHNode *rightChild, const Binning *bins)
{
    const int leftEnd = m_begin + bins[m_splitDim].countLeft(m_splitInd);
	leftChild->setAABB(splitLeftBox(bins));
	leftChild->setLeaf(m_begin, leftEnd);
	rightChild->setAABB(splitRightBox(bins));
	rightChild->setLeaf(leftEnd, m_end);
}

const int &BVHSplit::begin() const
{ return m_begin; }

const int &BVHSplit::end() const
{ return m_end; }

std::ostream& operator<<(std::ostream &output, const BVHSplit & p) 
{
	output << " split range [" << p.begin() << " " << p.end()
			<< "] non-split cost "<< p.getNonSplitCost()
			<< " split cost " << p.splitCost()
			<< " split axis "<< p.splitAxis();
    return output;
}

}
