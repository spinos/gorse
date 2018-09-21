/*
 *  Lehmer.h
 *  aloe
 *
 *  Lehmer random number generator
 *  X_k+1 <- g X_k mod n
 *  g = 48271
 *  n = 2^31 - 1
 *  used in C++11's minstd_rand
 *
 *  Created by jian zhang on 3/16/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_RNG_LEHMER_H
#define ALO_RNG_LEHMER_H

namespace alo {

template<unsigned int G, unsigned int N>
class LCG {

	unsigned int m_seed;
	
public:

	LCG(unsigned int seed);
	
	unsigned int rnd();

private:

};

template<unsigned int G, unsigned int N>
LCG<G, N>::LCG(unsigned int seed) : m_seed(seed)
{}

template<unsigned int G, unsigned int N>
unsigned int LCG<G, N>::rnd()
{ return m_seed = ((unsigned long long)m_seed * G) % N; }

typedef LCG<48271u, 0x7fffffff> Lehmer;

}

#endif