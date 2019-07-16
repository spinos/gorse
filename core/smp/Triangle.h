/*
 *  Triangle.h
 *  gorse  
 *
 *  randomly sample point on triangle (A,B,C) input (r1,r2) [0,1]
 *  P = (1 - sqrt(r1)) * A + (sqrt(r1) * (1 - r2)) * B + (sqrt(r1) * r2) * C
 *  based on Shape Distributions by ROBERT OSADA et al.
 *
 *  Created by jian zhang on 2/10/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SMP_TRIANGLE_H
#define ALO_SMP_TRIANGLE_H

#include <rng/Halton.h>
#include <math/miscfuncs.h>

namespace alo {

namespace smp {

template<typename T>
class Triangle {

    T m_v[3];
/// to interpolate
	float m_coord[3];
/// area of sample
	float m_area;
/// to vertices
	int m_ind[3];
	
public:

	Triangle();
	
	void setSampleSize(float x);
    
    T* vertex();
	int* index();
	
	const int* c_index() const;
	const float* c_coord() const;
	
/// relative to triangle area
	int getNumSamples() const;
	
/// Tf interpolater
	template<typename Tf, typename Tr>
	bool sampleTriangle(T& samp, Tf& finterp, Tr* prng);
    
    template<typename Ta, typename Tf, typename Tr >
	void addSamples(Ta& arr, T& samp, Tf& finterp, Tr* prng);
    
protected:
    
    template<typename Tr>
    void setCoord(Tr* prng);

};

template<typename T>
Triangle<T>::Triangle() :
m_area(1.f)
{}

template<typename T>
T* Triangle<T>::vertex()
{ return m_v; }

template<typename T>
int* Triangle<T>::index()
{ return m_ind; }

template<typename T>
void Triangle<T>::setSampleSize(float x)
{ m_area = x * x * .73f; }

template<typename T>
int Triangle<T>::getNumSamples() const
{
    Vector3F d[3]; float l[3];

	d[0] = m_v[1]._pos - m_v[0]._pos;
	l[0] = d[0].length();
	
	d[1] = m_v[2]._pos - m_v[1]._pos;
	l[1] = d[1].length();
	
	d[2] = m_v[0]._pos - m_v[2]._pos;
	l[2] = d[2].length();
    
    const float tarea = (0.5*sqrt(l[0]*l[0]*l[2]*l[2] - (d[0].dot(d[2]))*(d[0].dot(d[2]))));
    
    return 1 + tarea / m_area; 
}

template<typename T>
template<typename Tf, typename Tr>
bool Triangle<T>::sampleTriangle(T& samp, Tf& finterp, Tr* prng)
{
	setCoord<Tr>(prng);
    samp = m_v[0] * m_coord[0] 
				+ m_v[1] * m_coord[1] 
				+ m_v[2] * m_coord[2];
	
	if(finterp.reject(samp) )
		return false;
		
	finterp.interpolate(samp, m_coord, m_ind);
	return true;
}

template<typename T>
template<typename Ta, typename Tf, typename Tr >
void Triangle<T>::addSamples(Ta& arr, T& samp, Tf& finterp, Tr* prng)
{
    const int ns = getNumSamples();
	int n = ns;
	for(int i=0;i<1000;++i) {
		
		if(n < 1)
			return;
			
		if(!sampleTriangle<Tf >(samp, finterp, prng) )
			continue;
			
		arr.push_back(samp);
		n--;
	}
}

template<typename T>
template<typename Tr>
void Triangle<T>::setCoord(Tr* prng)
{
    const float* r1r2 = Halton::Rand2(prng);

	float r1 = r1r2[0];
	float r2 = r1r2[1];
	
	r1 = sqrt(r1);
	
	m_coord[0] = 1.f - r1;
	m_coord[1] = r1 * (1.f - r2);
	m_coord[2] = r1 * r2;
}

template<typename T>
const int* Triangle<T>::c_index() const
{ return m_ind; }

template<typename T>
const float* Triangle<T>::c_coord() const
{ return m_coord; }

}

}

#endif