/*
 *  ElementVector.h
 *  gorse
 *
 *  vector of pointers of type T
 *
 *  2019/4/24
 */

#ifndef ALO_ELEMENT_VECTOR_H
#define ALO_ELEMENT_VECTOR_H

#include <vector>

namespace alo {
    
template<typename T>
class ElementVector
{
    std::vector<T *> m_elms;
    bool m_isDirty;
    
public:
    ElementVector();
    virtual ~ElementVector();
    
    void clear();
    
    bool contains(T *x) const;
    void append(T *x);
    bool remove(T *x);
    size_t numElements() const;
    T * element(int i);
    const T *element(int i) const;
    T * back();
    const T *back() const;

    const bool &isDirty() const;
    void setClean();

    void sendImpulse();

protected:
    
private:
    
};

template<typename T>
ElementVector<T>::ElementVector() : m_isDirty(false)
{}

template<typename T>
ElementVector<T>::~ElementVector()
{}

template<typename T>
void ElementVector<T>::clear()
{ m_elms.clear(); }

template<typename T>
bool ElementVector<T>::contains(T *x) const
{ 
    if(numElements() < 1) return false;
    return std::find(m_elms.begin(), m_elms.end(), x) != m_elms.end(); 
}

template<typename T>
void ElementVector<T>::append(T *x)
{ 
    m_elms.push_back(x); 
    m_isDirty = true;
}

template<typename T>
bool ElementVector<T>::remove(T *x)
{
    std::vector<T *>::iterator it = std::find(m_elms.begin(), m_elms.end(), x);
    if(it==m_elms.end()) return false;
    m_elms.erase(it);
    m_isDirty = true;
    return true;
}

template<typename T>
size_t ElementVector<T>::numElements() const
{ return m_elms.size(); }

template<typename T>
T * ElementVector<T>::element(int i)
{ return m_elms[i]; }

template<typename T>
const T *ElementVector<T>::element(int i) const
{ return m_elms[i]; }

template<typename T>
const bool &ElementVector<T>::isDirty() const
{ return m_isDirty; }

template<typename T>
void ElementVector<T>::setClean()
{ m_isDirty = false; }

template<typename T>
void ElementVector<T>::sendImpulse()
{
    std::vector<T *>::iterator it = m_elms.begin();
    for(;it!=m_elms.end();++it) {
        (*it)->sendImpulse();
    }
}

template<typename T>
T *ElementVector<T>::back()
{ return m_elms.back(); }

template<typename T>
const T *ElementVector<T>::back() const
{ return m_elms.back(); }

} /// end of alo

#endif
