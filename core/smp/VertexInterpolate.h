/*
 *  VertexInterpolate.h
 *  aloe
 *
 *  Created by jian zhang on 3/12/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SMP_VERTEX_INTERPOLATE_H
#define ALO_SMP_VERTEX_INTERPOLATE_H

#include <boost/scoped_array.hpp>

namespace alo {

namespace smp {

template<typename T>
class VertexInterpolate {

	boost::scoped_array<T> m_values;
	int m_numVertices;
	
public:

	VertexInterpolate();
	
	void createNumVertices(int n);
	
	T* values();
	const T* c_values() const;
	
	template<typename Ts>
	bool reject(const Ts& asmp ) const;
	
	template<typename Ts>
	void interpolate(Ts& asmp,
			const float* coord,
			const int* v) const;
			
protected:

private:

};

template<typename T>
VertexInterpolate<T>::VertexInterpolate() : m_numVertices(0)
{}

template<typename T>
void VertexInterpolate<T>::createNumVertices(int n)
{ 
	m_values.reset(new T[n]); 
	m_numVertices = n;
}

template<typename T>
T* VertexInterpolate<T>::values()
{ return m_values.get(); }

template<typename T>
const T* VertexInterpolate<T>::c_values() const
{ return m_values.get(); }

template<typename T>
template<typename Ts>
bool VertexInterpolate<T>::reject(const Ts& asmp ) const 
{ return false; }

template<typename T>
template<typename Ts>
void VertexInterpolate<T>::interpolate(Ts& asmp,
			const float* coord,
			const int* v) const 
{}

}

}

#endif
