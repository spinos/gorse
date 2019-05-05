/*
 *  SpaceFillingVector.h
 *  aloe
 *  
 *  T is element type
 *  Tc is space filling curve type
 *
 *  Created by jian zhang on 2/12/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SDS_SPACE_FILLING_VECTOR_H
#define ALO_SDS_SPACE_FILLING_VECTOR_H

#include <vector>
#include <math/QuickSort.h>

namespace alo {

namespace sds {

template<typename T>
class SpaceFillingVector {

	std::vector<T> m_v;
	
public:

	typedef T ValueTyp;

	SpaceFillingVector();
	virtual ~SpaceFillingVector();
	
	void push_back(const T& x);
	void clear();
	int size() const;
/// encode and sort
/// Tc is  space filling curve type
	template<typename Tc>
	void createSFC(const Tc& sfcf);
	
	template<typename Tc>
	void createSFCAtLevel(const Tc& sfcf, int level);
	
	void sort();
	
/// where a =< key < b
	bool searchSFC(int* dst, const int& a, const int& b) const;
	bool searchSFC(int* dst, const int* range) const;
    
	int findElement(const int& k) const;
	
	T& operator[](int i);
	const T& operator[](int i) const;
	
protected:
	
	int searchElementNoGreaterThan(int first, int last, const int& thre) const;
	int searchElementNoLessThan(int first, int last, const int& thre) const;
	
private:

};

template<typename T>
SpaceFillingVector<T>::SpaceFillingVector()
{}

template<typename T>
SpaceFillingVector<T>::~SpaceFillingVector()
{ clear(); }

template<typename T>
void SpaceFillingVector<T>::push_back(const T& x)
{ m_v.push_back(x); }

template<typename T>
void SpaceFillingVector<T>::clear()
{ m_v.clear(); }

template<typename T>
int SpaceFillingVector<T>::size() const
{ return m_v.size(); }

template<typename T>
T& SpaceFillingVector<T>::operator[](int i)
{ return m_v[i]; }
	
template<typename T>
const T& SpaceFillingVector<T>::operator[](int i) const
{ return m_v[i]; }

template<typename T>
template<typename Tc>
void SpaceFillingVector<T>::createSFC(const Tc& sfcf)
{
	const int n = m_v.size();
	for(int i=0;i<n;++i) {
		T& vi = m_v[i];
		vi._key = sfcf.computeKey((const float*)&vi._pos);
	}
	sort();
}	

template<typename T>
template<typename Tc>
void SpaceFillingVector<T>::createSFCAtLevel(const Tc& sfcf, int level)
{
	const int n = m_v.size();
	for(int i=0;i<n;++i) {
		T& vi = m_v[i];
		vi._key = sfcf.computeKeyAtLevel((const float*)&vi._pos, level);
	}
	sort();
}	

template<typename T>
void SpaceFillingVector<T>::sort()
{
	const int n = m_v.size();
	QuickSort1::SortVector<int, T>(m_v, 0, n-1);
}

template<typename T>
bool SpaceFillingVector<T>::searchSFC(int* dst, const int* range) const
{ return searchSFC(dst, range[0], range[1]); }

template<typename T>
bool SpaceFillingVector<T>::searchSFC(int* dst, const int& a, const int& b) const
{
    const int n = m_v.size();
	
	if(m_v[0]._key >= b || m_v[n-1]._key < a) {
		dst[0] = 0;
		dst[1] = -1;
		return false;
	}
	
	dst[0] = searchElementNoGreaterThan(0, n-1, a);
	dst[1] = searchElementNoLessThan(dst[0], n-1, b);
	return (dst[1] > dst[0]+1);
}

template<typename T>
int SpaceFillingVector<T>::searchElementNoGreaterThan(int first, int last, const int& thre) const
{
	int mid;
	while(last > first + 1) {
		
		mid = (first + last) / 2;
		
		const int& r = m_v[mid]._key;
		if(r == thre)
			return mid;
		
		if(r > thre)
			last = mid;
		else
			first = mid;
		
	}
	
	return first;
}

template<typename T>
int SpaceFillingVector<T>::searchElementNoLessThan(int first, int last, const int& thre) const
{
	int mid;
	while(last > first + 1) {
		
		mid = (first + last) / 2;
		
		const int& r = m_v[mid]._key;
		if(r == thre)
			return mid;
		
		if(r > thre)
			last = mid;
		else
			first = mid;
		
	}
	
	return last;
}

template<typename T>
int SpaceFillingVector<T>::findElement(const int& k) const
{
	if(m_v[0]._key == k)
		return 0;
		
	const int n = m_v.size();
	if(m_v[n-1]._key == k)
		return n-1;
		
	int i = searchElementNoLessThan(0, n-1, k);
	if(m_v[i]._key == k)
		return i;
		
	if(m_v[i-1]._key == k)
		return i-1;
		
	return -1;
}

}

}

#endif