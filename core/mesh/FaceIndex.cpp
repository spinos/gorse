#include "FaceIndex.h"

namespace alo {

FaceIndex::FaceIndex() 
{ m_v0 = m_v1 = m_v2 = 0; }

FaceIndex::FaceIndex(int a, int b, int c) 
{ 
	m_v0 = a;
	if(b < m_v0) m_v0 = b;
	if(c < m_v0) m_v0 = c;

	m_v2 = a;
	if(b > m_v2) m_v2 = b;
	if(c > m_v2) m_v2 = c;

	m_v1 = a;
	if(b > m_v0 && b < m_v2) m_v1 = b;
	if(c > m_v0 && c < m_v2) m_v1 = c;
}

bool FaceIndex::isValid() const
{
	return (m_v2 > m_v1 && m_v1 > m_v0);
}

const int &FaceIndex::v0() const
{ return m_v0; }

const int &FaceIndex::v1() const
{ return m_v1; }

const int &FaceIndex::v2() const
{ return m_v2; }

int &FaceIndex::v0()
{ return m_v0; }

int &FaceIndex::v1()
{ return m_v1; }

int &FaceIndex::v2()
{ return m_v2; }

const bool FaceIndex::operator==(const FaceIndex & another) const {
	return v0() == another.v0() && v1() == another.v1() && v2() == another.v2();
}

const bool FaceIndex::operator<(const FaceIndex & another) const {
	if(v2() < another.v2()) return true;
	if(v2() > another.v2()) return false;
/// z equals
	if(v1() < another.v1()) return true;
	if(v1() > another.v1()) return false;
/// y equals
	return v0() < another.v0();
}

const bool FaceIndex::operator>=(const FaceIndex & another) const {
	if(v2() > another.v2()) return true;
	if(v2() < another.v2()) return false;
/// z equals
	if(v1() > another.v1()) return true;
	if(v1() < another.v1()) return false;
/// y equals
	return v0() >= another.v0();
}

const bool FaceIndex::operator>(const FaceIndex & another) const {
	if(v2() > another.v2()) return true;
	if(v2() < another.v2()) return false;
/// z equals
	if(v1() > another.v1()) return true;
	if(v1() < another.v1()) return false;
/// y equals
	return v0() > another.v0();
}

bool FaceIndex::hasV(int x) const
{ return (v0() == x || v1() == x || v2() == x); }

}
