/*
 *  convolute.h
 *  aloe math
 *
 *  Created by jian zhang on 4/1/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

namespace alo {

template<typename T>
static inline void convolute(T* b, const T* a, int n, const T* kern, int ntap) 
{
	const int nshift = -ntap / 2;
	
	for(int i=0;i<n;++i) {
	
		b[i] = 0;
		
		for(int j=0;j<ntap;++j) {
		
			int s = i + j + nshift;
			if(s < 0) s = 0;
			if(s > n-1) s = n - 1;
			
			b[i] += a[s] * kern[j];
		}
	}
}

}