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

	int m_numSplits;
	float *m_splitPos;
	int *m_objectInBin;
	int *m_countLeft;
	int *m_countRight;
	BoundingBox *m_aabbInBin;
	BoundingBox *m_aabbLeft;
	BoundingBox *m_aabbRight;

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
