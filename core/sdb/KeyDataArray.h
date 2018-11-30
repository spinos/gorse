/*
 *  KeyDataArray.h
 *  sdb
 *
 *  SoA (key,data) look up
 *
 */

#ifndef SDB_KEY_DATA_ARRAY_H
#define SDB_KEY_DATA_ARRAY_H

#include <iostream>

namespace alo {

namespace sdb {

template <typename KeyType, typename DataType, int Dim>
class KeyDataArray {

	KeyType m_key[Dim];
	DataType m_data[Dim];
	int m_count;

public:
	KeyDataArray();
    virtual ~KeyDataArray();

    DataType *find(const KeyType &x);
    int insert(const KeyType &x, const DataType &a, bool writeExisting=true);
    bool remove(const KeyType &x);
    void copyFrom(const KeyType *ks, const DataType *vs, int n);
    void add(const DataType &x);

    const int &count() const;
    bool isFull() const;
    bool isSingular() const;
    bool isEmpty() const;

    const KeyType &key(int i) const;
    const DataType &value(int i) const;
    KeyType &key(int i);
    DataType &value(int i);

/// last b <= x -1 if all > x
    int keyLeft(const KeyType & x, KeyType &b) const;
/// first b > x -1 if all < x
    int keyRight(const KeyType & x, KeyType &b) const;

protected:

	const KeyType *keys() const;
	const DataType *values() const;
	void setCount(int x);

};

template <typename KeyType, typename DataType, int Dim>
KeyDataArray<KeyType, DataType, Dim>::KeyDataArray() :
m_count(0)
{}

template <typename KeyType, typename DataType, int Dim>
KeyDataArray<KeyType, DataType, Dim>::~KeyDataArray() 
{}

template <typename KeyType, typename DataType, int Dim>
DataType *KeyDataArray<KeyType, DataType, Dim>::find(const KeyType &x)
{
	if(isEmpty()) return NULL;
	KeyType b;
	int loc = keyLeft(x, b);
	if(loc < 0) return NULL;
	if(!(b == x)) return NULL;

	return &m_data[loc];
}

template <typename KeyType, typename DataType, int Dim>
int KeyDataArray<KeyType, DataType, Dim>::insert(const KeyType &x, const DataType &a, bool writeExisting)
{
	if(isEmpty()) {
		m_key[0] = x;
		m_data[0] = a;
		m_count++;
		return 0;
	}
	
	KeyType b;
	int loc = keyLeft(x, b);
	if(b == x && loc > -1) {
		if(writeExisting) m_data[loc] = a;
		return loc;
	}

	int n = m_count - (loc + 1);
	if(n < 1) {
		m_key[m_count] = x;
		m_data[m_count] = a;
		m_count++;
		return m_count-1;
	}

	KeyType *bk = new KeyType[n];
	memcpy(bk, &m_key[loc+1], n * sizeof(KeyType));
	memcpy(&m_key[loc+2], bk, n * sizeof(KeyType));
	delete[] bk;

	DataType *bd = new DataType[n];
	memcpy(bd, &m_data[loc+1], n * sizeof(DataType));
	memcpy(&m_data[loc+2], bd, n * sizeof(DataType));
	delete[] bd;

	m_key[loc+1] = x;
	m_data[loc+1] = a;

	m_count++;
	return loc+1;

}

template <typename KeyType, typename DataType, int Dim>
void KeyDataArray<KeyType, DataType, Dim>::copyFrom(const KeyType *ks, const DataType *vs, int n)
{
	memcpy(m_key, ks, n * sizeof(KeyType));
	memcpy(m_data, vs, n * sizeof(DataType));
	m_count = n;
}

template <typename KeyType, typename DataType, int Dim>
void KeyDataArray<KeyType, DataType, Dim>::add(const DataType &x)
{
	for(int i=0;i<m_count;++i)
		m_data[i] += x;
}

template <typename KeyType, typename DataType, int Dim>
bool KeyDataArray<KeyType, DataType, Dim>::remove(const KeyType &x)
{
	if(isEmpty()) return false;

	KeyType b;
	int loc = keyLeft(x, b);
	if(loc < 0) return false;
	if(!(b == x)) return false;

	int n = m_count - (loc + 1);
	if(n < 1) {
		m_count--;
		return true;
	}

	KeyType *bk = new KeyType[n];
	memcpy(bk, &m_key[loc+1], n * sizeof(KeyType));
	memcpy(&m_key[loc], bk, n * sizeof(KeyType));
	delete[] bk;

	DataType *bd = new DataType[n];
	memcpy(bd, &m_data[loc+1], n * sizeof(DataType));
	memcpy(&m_data[loc], bd, n * sizeof(DataType));
	delete[] bd;

	m_count--;
	return true;
}

template <typename KeyType, typename DataType, int Dim>
const KeyType &KeyDataArray<KeyType, DataType, Dim>::key(int i) const
{ return m_key[i]; }

template <typename KeyType, typename DataType, int Dim>
const DataType &KeyDataArray<KeyType, DataType, Dim>::value(int i) const
{ return m_data[i]; }

template <typename KeyType, typename DataType, int Dim>
KeyType &KeyDataArray<KeyType, DataType, Dim>::key(int i)
{ return m_key[i]; }

template <typename KeyType, typename DataType, int Dim>
DataType &KeyDataArray<KeyType, DataType, Dim>::value(int i)
{ return m_data[i]; }

template <typename KeyType, typename DataType, int Dim>
const KeyType *KeyDataArray<KeyType, DataType, Dim>::keys() const
{ return m_key; }

template <typename KeyType, typename DataType, int Dim>
const DataType *KeyDataArray<KeyType, DataType, Dim>::values() const
{ return m_data; }

template <typename KeyType, typename DataType, int Dim>
const int &KeyDataArray<KeyType, DataType, Dim>::count() const
{ return m_count; }

template <typename KeyType, typename DataType, int Dim>
bool KeyDataArray<KeyType, DataType, Dim>::isFull() const
{ return m_count > Dim-2; }

template <typename KeyType, typename DataType, int Dim>
bool KeyDataArray<KeyType, DataType, Dim>::isSingular() const
{ return m_count == 1; }

template <typename KeyType, typename DataType, int Dim>
bool KeyDataArray<KeyType, DataType, Dim>::isEmpty() const
{ return m_count < 1; }

template <typename KeyType, typename DataType, int Dim>
int KeyDataArray<KeyType, DataType, Dim>::keyLeft(const KeyType & x, KeyType &b) const
{
	int low = 0;
	if(m_key[low] > x)
		return -1;

	if(isSingular()) {
		b = m_key[0];
		return 0;
	}
	
	int high = m_count - 1;
	if(m_key[high] <= x) {
		b = m_key[high];
		return high;
	}

	while(high > low + 1) {
		int mid = (low + high)>>1;
		if(m_key[mid] == x) {
			b = m_key[mid];
			return mid;
		}

		if(m_key[mid] < x) low = mid;
		else high = mid;
	}

	b = m_key[low];
	return low;
}

template <typename KeyType, typename DataType, int Dim>
int KeyDataArray<KeyType, DataType, Dim>::keyRight(const KeyType & x, KeyType &b) const
{
	int low = 0;
	if(m_key[low] > x) {
		b = m_key[low];
		return 0;
	}

	if(isSingular())
		return -1;

	int high = m_count - 1;
	if(m_key[high] <= x)
		return -1;
	
	while(high > low + 1) {
		int mid = (low + high)>>1;
		if(m_key[mid] == x) {
			b = m_key[mid+1];
			return mid+1;
		}

		if(m_key[mid] < x) low = mid;
		else high = mid;
	}

	b = m_key[high];
	return high;
}

template <typename KeyType, typename DataType, int Dim>
void KeyDataArray<KeyType, DataType, Dim>::setCount(int x)
{ m_count = x; }

}

}

#endif
