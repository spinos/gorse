/*
 * Binning.h
 *
 * 0|1|2|...|n-1|n   
 *  n + 1 bin
 *  n split
 *
 *  1 0 1 4 3 0 0  objests in bin
 *   1 1 2 6 9 9   left to split
 *   8 8 7 3 0 0   right to split
 * 
 */

#ifndef ALO_BVH_BINNING_H
#define ALO_BVH_BINNING_H

#include <math/BoundingBox.h>

namespace alo {

class Binning {
	
#define MAX_NUM_SPLIT 32
#define MAX_NUM_OBJECT 33

	int m_numSplits;
	float m_splitPos[MAX_NUM_SPLIT];
	int m_objectInBin[MAX_NUM_OBJECT];
	int m_countLeft[MAX_NUM_SPLIT];
	int m_countRight[MAX_NUM_SPLIT];
	BoundingBox m_aabbInBin[MAX_NUM_OBJECT];
	BoundingBox m_aabbLeft[MAX_NUM_SPLIT];
	BoundingBox m_aabbRight[MAX_NUM_SPLIT];

public:

	Binning();
	~Binning();

	void setEmpty();
	void create(int n);
	void setSplitPos(float x, int i);
	void count(float x, const BoundingBox &box);
	void scan();

	bool isEmpty() const;
	const int &numSplits() const;
	const float &splitPos(int i) const;
	const int &countInBin(int i) const;
	const int &countLeft(int i) const;
	const int &countRight(int i) const;
	const BoundingBox &aabbLeft(int i) const;
	const BoundingBox &aabbRight(int i) const;

	friend std::ostream& operator<<(std::ostream &output, const Binning & p);

private:
	int getBin(float x) const;

};

}

#endif
