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

	void connectToFace(const FaceIndex &x);
	bool isOnBorder() const;
	
	float &cost();
	const float &cost() const;

	friend std::ostream& operator<<(std::ostream &output, const EdgeValue & p)
    {
        output << " " << p.cost();
        return output;
    }
	
};

}

#endif
