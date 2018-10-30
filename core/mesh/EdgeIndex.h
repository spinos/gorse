#ifndef ALO_EDGE_INDEX_H
#define ALO_EDGE_INDEX_H

#include <iostream>

namespace alo {
	
class EdgeIndex
{
	unsigned m_v0, m_v1;

public:
	EdgeIndex();
	EdgeIndex(int a, int b);

	bool isValid() const;
	bool hasV(int x) const;
	unsigned &v0();
	unsigned &v1();
	const unsigned &v0() const;
	const unsigned &v1() const;

	const bool operator==(const EdgeIndex & another) const;
	const bool operator!=(const EdgeIndex & another) const;
	const bool operator<(const EdgeIndex & another) const;
	const bool operator>(const EdgeIndex & another) const;
	const bool operator>=(const EdgeIndex & another) const;
	const bool operator<=(const EdgeIndex & another) const;
	
	friend std::ostream& operator<<(std::ostream &output, const EdgeIndex & p)
    {
        output << " (" << p.v0() << ", " << p.v1() << ") ";
        return output;
    }
	
};

}

#endif
