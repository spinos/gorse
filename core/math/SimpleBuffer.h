/*
 * SimpleBuffer
 * aloe
 *
 * extend storage by memcpy
 *
 */

#ifndef ALO_MATH_SIMPLE_BUFFER_H
#define ALO_MATH_SIMPLE_BUFFER_H

#include <iostream>
#ifdef LINUX
#include <string.h>
#endif

namespace alo {

template<typename T>
class SimpleBuffer {
    
    T* m_data;
/// num of elements
    int m_count;
    int m_cap;  
    
public:

    SimpleBuffer();
    ~SimpleBuffer();
    
    void createBuffer(int count);
    void resetBuffer(int count);
/// insert n elements at loc
    void insert(const T *b, int n, int loc);
/// swap n elements between location a and b
    void swap(int a, int b, int n);
    
    T* data();
    const T* c_data() const;

    T &last();
    
    T& operator[](int i);
    const T& operator[](int i) const;
    const int &count() const;
/// max num of elements
    const int &capacity() const;
    int capacityByteSize() const;

    void operator<<(const T& x);
    
private:

    int calcCap(int count) const;
    void extendBuffer(int count);
    
};

template<typename T>
SimpleBuffer<T>::SimpleBuffer() : m_data(0),
    m_count(0), 
    m_cap(0)
{}

template<typename T>
SimpleBuffer<T>::~SimpleBuffer()
{
    if(m_data) delete[] m_data;
}

template<typename T>
void SimpleBuffer<T>::resetBuffer(int count)
{
    if(m_data)
        delete[] m_data;

    m_cap = 4096;
    m_data = new T[m_cap];
    m_count = count;
}

template<typename T>
void SimpleBuffer<T>::createBuffer(int count)
{
    if(m_cap < count)
        extendBuffer(count);
    m_count = count;
}

template<typename T>
void SimpleBuffer<T>::extendBuffer(int count)
{
    m_cap = calcCap(count);
    T* d = new T[m_cap];
    if(m_data) {
        memcpy(d, m_data, sizeof(T) * m_count);
        delete[] m_data;
    }
    m_data = d;
}

template<typename T>
int SimpleBuffer<T>::calcCap(int count) const
{
    int r = count >> 12;
    if(count & 4095) r++;
    return (r<<12);
}

template<typename T>
T* SimpleBuffer<T>::data()
{ return m_data; }

template<typename T>
const T* SimpleBuffer<T>::c_data() const
{ return m_data; }

template<typename T>
T& SimpleBuffer<T>::operator[](int i)
{ return m_data[i]; }

template<typename T>
const T& SimpleBuffer<T>::operator[](int i) const
{ return m_data[i]; }

template<typename T>
const int &SimpleBuffer<T>::count() const
{ return m_count; }

template<typename T>
const int &SimpleBuffer<T>::capacity() const
{ return m_cap; }

template<typename T>
int SimpleBuffer<T>::capacityByteSize() const
{ return m_cap * sizeof(T); }

template<typename T>
T &SimpleBuffer<T>::last()
{ return m_data[m_count - 1]; }

template<typename T>
void SimpleBuffer<T>::operator<<(const T& x)
{
    int c1 = m_count + 1;
    if(m_cap < c1)
        extendBuffer(c1);
    m_data[m_count] = x;
    m_count = c1;
}

template<typename T>
void SimpleBuffer<T>::insert(const T *b, int n, int loc)
{
    int c1 = m_count + n;
    if(m_cap < c1)
        extendBuffer(c1);

    int bn = m_count - loc;
    T *c = new T[bn];
    const size_t cz = sizeof(T) * bn;

    memcpy(c, &m_data[loc], cz);

    memcpy(&m_data[loc], b, sizeof(T) * n);

    memcpy(&m_data[loc + n], c, cz);

    m_count += n;
    delete[] c;
}

template<typename T>
void SimpleBuffer<T>::swap(int a, int b, int n)
{
    T *c = new T[n];
    const int cz = n * sizeof(T);

    memcpy(c, &m_data[a], cz);

    memcpy(&m_data[a], &m_data[b], cz);

    memcpy(&m_data[b], c, cz);

    delete[] c;
}

}

#endif