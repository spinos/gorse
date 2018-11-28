/*
 *  Types.h
 *  aloe
 *
 */

#ifndef ALO_SDB_TYPES_H
#define ALO_SDB_TYPES_H

#include <iostream>
#include <string>

namespace alo {

namespace sdb {

class Coord2
{
public:
	Coord2();
	Coord2(int a, int b);
	const bool operator==(const Coord2 & another) const;
	const bool operator<(const Coord2 & another) const;
	const bool operator<=(const Coord2 & another) const;
	const bool operator>=(const Coord2 & another) const;
	const bool operator>(const Coord2 & another) const;
	Coord2 ordered() const;
	const std::string str() const;
	friend std::ostream& operator<<(std::ostream &output, const Coord2 & p)
    {
        output << p.str();
        return output;
    }
	int x, y;
};

class Coord3
{
public:
	Coord3();
	Coord3(int a, int b, int c);
	const bool operator==(const Coord3 & another) const;
	const bool operator<(const Coord3 & another) const;
	const bool operator>=(const Coord3 & another) const;
	const bool operator>(const Coord3 & another) const;
    Coord3 operator+(const Coord3 & another) const;
	Coord3 ordered() const;
/// z as highest and keep in order
	void makeUnique();
	const std::string str() const;
	friend std::ostream& operator<<(std::ostream &output, const Coord3 & p)
    {
        output << p.str();
        return output;
    }
	
	bool intersects(const int & v) const;
	bool intersects(const Coord2 & e) const;
	
	int x, y, z;
};

template <typename KeyType, typename IndexType> 
class Pair
{
public:
	Pair() { index = NULL; }
	KeyType key;
	IndexType * index;
	friend std::ostream& operator<<(std::ostream &output, const Pair & p)
    {
        output << p.key << " : " << *p.index;
        return output;
    }
};

template<typename T1, typename T2>
class Couple
{
public:
	T1 * t1;
	T2 * t2;
	
	Couple() 
	{
		t1 = new T1;
		t2 = new T2;
	}
};

template<typename T1, typename T2, typename T3>
class Triple
{
public:
	T1 * t1;
	T2 * t2;
	T3 * t3;
	
	Triple()
	{
		t1 = new T1;
		t2 = new T2;
		t3 = new T3;
	}
};

template<typename T1, typename T2, typename T3, typename T4>
class Quadruple
{
public:
	T1 * t1;
	T2 * t2;
	T3 * t3;
	T4 * t4;
	
	Quadruple()
	{
		t1 = new T1;
		t2 = new T2;
		t3 = new T3;
		t4 = new T4;
	}
};

} // end namespace sdb

}
#endif
