#ifndef ALO_FACE_VALUE_H
#define ALO_FACE_VALUE_H

#include <iostream>

namespace alo {

class Vector3F;
class FaceIndex;

class FaceValue
{
	int m_ind;
	float m_area;

public:
	FaceValue();
	FaceValue(int x);
	
	void calculateArea(const Vector3F *pos, const FaceIndex &fi);
	int &ind();
	const int &ind() const;
	const float &area() const;

	friend std::ostream& operator<<(std::ostream &output, const FaceValue & p)
    {
        output << " " << p.ind();
        return output;
    }
	
};

}

#endif
