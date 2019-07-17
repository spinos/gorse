/*
 *  SampleFilter.h
 *  aloe
 *
 *  select part of the T samples
 *
 *  2019/7/17
 */
 
#ifndef ALO_SAMPLE_FILTER_H
#define ALO_SAMPLE_FILTER_H

#include <math/SimpleBuffer.h>

namespace alo {
    
namespace smp {
    
template<typename T>
class SampleFilter {
    
    SimpleBuffer<T> m_samples;
    
public:
    
    SampleFilter();
    virtual ~SampleFilter();
	
	void create(int n);
	void clear();
	
	T *samples();
	
	const int &numSamples() const;
    
protected:
    
private:
    
};

template<typename T>
SampleFilter<T>::SampleFilter()
{}

template<typename T>
SampleFilter<T>::~SampleFilter()
{}

template<typename T>
void SampleFilter<T>::create(int n)
{ m_samples.resetBuffer(n); }

template<typename T>
void SampleFilter<T>::clear()
{ m_samples.purgeBuffer(); }

template<typename T>
T *SampleFilter<T>::samples()
{ return m_samples.data(); }

template<typename T>
const int &SampleFilter<T>::numSamples() const
{ return m_samples.count(); }

}
    
}

#endif

