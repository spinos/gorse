/*
 *  NamedBufferArray.h
 *  aloe
 *
 *  buffer[N] of type T of the same size
 *  access by index or name
 *
 */

#ifndef ALO_NAMED_BUFFER_ARRAY_H
#define ALO_NAMED_BUFFER_ARRAY_H

#include "SimpleBuffer.h"
#include <string>

namespace alo {

template<typename T, int N>
class NamedBufferArray {

	std::string m_names[N];
	SimpleBuffer<T> m_buf[N];
	int m_count;

public:
	NamedBufferArray();
	~NamedBufferArray();

/// n buffers of size d
	void resize(int n, int d);
	void clear();
	void setName(const std::string &name, int i);

	const int &count() const;
	SimpleBuffer<T> *buffer(int i);

	const std::string &c_name(int i) const;
	const SimpleBuffer<T> *c_buffer(int i) const;
	int findName(const std::string &name) const;

private:
	void shrinkTo(int n);

};

template<typename T, int N>
NamedBufferArray<T, N>::NamedBufferArray() :
m_count(0)
{}

template<typename T, int N>
NamedBufferArray<T, N>::~NamedBufferArray()
{}

template<typename T, int N>
void NamedBufferArray<T, N>::resize(int n, int d)
{
	if(n < 1) {
		clear();
		return;
	}

	int rn = n;
	if(rn > N) rn = N;

	if(m_count > rn) shrinkTo(rn);
	if(m_count < rn) m_count = rn;
	
	for(int i=0;i<m_count;++i)
		m_buf[i].resetBuffer(d);

}

template<typename T, int N>
void NamedBufferArray<T, N>::clear()
{
	for(int i=0;i<m_count;++i)
		m_buf[i].purgeBuffer();

	m_count = 0;
}

template<typename T, int N>
void NamedBufferArray<T, N>::setName(const std::string &name, int i)
{ m_names[i] = name; }

template<typename T, int N>
const int &NamedBufferArray<T, N>::count() const
{ return m_count; }

template<typename T, int N>
SimpleBuffer<T> *NamedBufferArray<T, N>::buffer(int i)
{ return &m_buf[i]; }

template<typename T, int N>
const std::string &NamedBufferArray<T, N>::c_name(int i) const
{ return m_names[i]; }

template<typename T, int N>
const SimpleBuffer<T> *NamedBufferArray<T, N>::c_buffer(int i) const
{ return &m_buf[i]; }

template<typename T, int N>
int NamedBufferArray<T, N>::findName(const std::string &name) const
{
	for(int i=0;i<m_count;++i) {
		if(m_names[i] == name) 
			return i;
	}
	return -1; 
}

template<typename T, int N>
void NamedBufferArray<T, N>::shrinkTo(int n)
{
	for(int i=m_count-1;i>n-1;--i) {
		m_names[i] = "unknown";
		m_buf[i].purgeBuffer();
	}
	m_count = n;
}

}

#endif
