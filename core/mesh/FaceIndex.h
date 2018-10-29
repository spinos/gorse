#ifndef ALO_FACE_INDEX_H
#define ALO_FACE_INDEX_H

#include <iostream>

namespace alo {
	
class FaceIndex
{
	int m_v0, m_v1, m_v2;

public:

	FaceIndex();
	FaceIndex(int a, int b, int c);

	bool isValid() const;
	const int &v0() const;
	const int &v1() const;
	const int &v2() const;
	int &v0();
	int &v1();
	int &v2();

	const bool operator==(const FaceIndex & another) const;
	const bool operator<(const FaceIndex & another) const;
	const bool operator>=(const FaceIndex & another) const;
	const bool operator>(const FaceIndex & another) const;

	bool hasV(int x) const;
	
	friend std::ostream& operator<<(std::ostream &output, const FaceIndex & p)
    {
        output << " (" << p.v0() << ", " << p.v1() << ", " << p.v2() << ") ";
        return output;
    }
	
};

}

#endif
