/*
 *  EdgeValue.h
 *  aloe
 *
 *  edge-to-face indirection
 *  edge index
 *
 */

#ifndef ALO_EDGE_VALUE_H
#define ALO_EDGE_VALUE_H

#include "FaceIndex.h"

namespace alo {

class EdgeIndex;
	
class EdgeValue
{
	FaceIndex m_face0, m_face1;
    float m_len;
	int m_ind;

public:
	EdgeValue();

	bool connectToFace(const FaceIndex &x);
	bool disconnectFace(const FaceIndex &x);
	bool isOnBorder() const;

	void setInd(int x);
    void setLength(float x);
	const int &ind() const;
    const float &length() const;
	const FaceIndex &face0() const;
	const FaceIndex &face1() const;

	friend std::ostream& operator<<(std::ostream &output, const EdgeValue & p);
	
};

}

#endif
