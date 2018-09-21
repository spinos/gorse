/*
 *  Uniform.h
 *  aloe
 *
 *  T rng
 *
 *  Created by jian zhang on 3/16/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_RNG_UNIFORM_H
#define ALO_RNG_UNIFORM_H

namespace alo {

template<typename T>
class Uniform {

	T* m_rng;
	
public:
	
	Uniform(unsigned int seed);
	~Uniform();

/// [0, imax]
	int rand(int imax);
/// [a, b] divided by imax
	float randf(float a, float b, int imax);
/// [0,1]
	float randf1();
/// 50-50 chance
	bool binary();
	
};

template<typename T>
Uniform<T>::Uniform(unsigned int seed)
{ m_rng = new T(seed); }

template<typename T>
Uniform<T>::~Uniform()
{ delete m_rng; }

template<typename T>
int Uniform<T>::rand(int imax)
{ return m_rng->rnd() % imax; }

template<typename T>
float Uniform<T>::randf(float a, float b, int imax)
{ return a + (float)rand(imax) / (float)imax * (b - a); }

template<typename T>
float Uniform<T>::randf1()
{ return (float)m_rng->rnd() / 2147483647.f; }

template<typename T>
bool Uniform<T>::binary()
{ return m_rng->rnd() < 1073741824; }

}

#endif
