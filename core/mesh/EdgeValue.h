#ifndef ALO_EDGE_VALUE_H
#define ALO_EDGE_VALUE_H

#include <iostream>
#include "FaceIndex.h"

namespace alo {
	
class EdgeValue
{
	float m_cost;
	FaceIndex m_face0, m_face1;

public:
	EdgeValue();

	bool connectToFace(const FaceIndex &x);
	bool disconnectFace(const FaceIndex &x);
	bool isOnBorder() const;
	
	float &cost();
	const float &cost() const;
	const FaceIndex &face0() const;
	const FaceIndex &face1() const;

	friend std::ostream& operator<<(std::ostream &output, const EdgeValue & p);
	
};

}

#endif
