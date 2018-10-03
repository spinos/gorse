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
    
    T* data();
    const T* c_data() const;
    
    T& operator[](int i);
    const T& operator[](int i) const;
    
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

}

#endif