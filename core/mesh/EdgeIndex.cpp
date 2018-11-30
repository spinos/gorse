#include "EdgeIndex.h"

namespace alo {

EdgeIndex::EdgeIndex() 
{ m_v0 = m_v1 = 0;}

EdgeIndex::EdgeIndex(int a, int b) 
{ 
	if(a < b) {
		m_v0 = a; 
		m_v1 = b; 
	} else {
		m_v0 = b; 
		m_v1 = a; 
	}
}

bool EdgeIndex::isValid() const
{ return m_v1 > m_v0; }

unsigned &EdgeIndex::v0()
{ return m_v0; }

unsigned &EdgeIndex::v1()
{ return m_v1; }

const unsigned &EdgeIndex::v0() const
{ return m_v0; }

const unsigned &EdgeIndex::v1() const
{ return m_v1; }

const bool EdgeIndex::operator==(const EdgeIndex &another) const 
{ 
	if(m_v0 != another.v0()) return false; 
	return m_v1 == another.v1(); 
}

const bool EdgeIndex::operator<(const EdgeIndex &another) const 
{
	if(m_v1 < another.v1()) return true;
	if(m_v1 > another.v1()) return false;
	
	return m_v0 < another.v0();
}

const bool EdgeIndex::operator>(const EdgeIndex &another) const 
{
	if(m_v1 > another.v1()) return true;
	if(m_v1 < another.v1()) return false;
	
	return m_v0 > another.v0();
}

const bool EdgeIndex::operator>=(const EdgeIndex &another) const 
{
	if(m_v1 > another.v1()) return true;
	if(m_v1 < another.v1()) return false;
	
	return m_v0 >= another.v0();
}

const bool EdgeIndex::operator<=(const EdgeIndex & another) const
{
	if(m_v1 < another.v1()) return true;
	if(m_v1 > another.v1()) return false;
	
	return m_v0 <= another.v0();
}

bool EdgeIndex::hasV(int x) const
{ 
	if(v0() == x) return true;
	return v1() == x; 
}

}
