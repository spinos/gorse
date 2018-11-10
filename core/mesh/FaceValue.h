#ifndef ALO_FACE_VALUE_H
#define ALO_FACE_VALUE_H

#include <iostream>
#include <math/Vector3F.h>

namespace alo {

class FaceIndex;

class FaceValue
{
	Vector3F m_nml;
	int m_ind;
	float m_area;

public:
	FaceValue();
	FaceValue(int x);
	
	void setNormal(const Vector3F &nml);
	void setArea(const float &x);
	int &ind();
	const int &ind() const;
	const float &area() const;
	const Vector3F &normal() const;

	friend std::ostream& operator<<(std::ostream &output, const FaceValue & p)
    {
        output << " " << p.ind();
        return output;
    }
	
};

}

#endif
