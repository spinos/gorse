#ifndef ALO_FACE_VALUE_H
#define ALO_FACE_VALUE_H

#include <iostream>

namespace alo {

class FaceValue
{
	int m_ind;

public:
	FaceValue();
	FaceValue(int x);
	
	int &ind();
	const int &ind() const;

	friend std::ostream& operator<<(std::ostream &output, const FaceValue & p)
    {
        output << " " << p.ind();
        return output;
    }
	
};

}

#endif
