/*
 *  PosSample.h
 *  aloe
 *
 *  Created by jian zhang on 2/15/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef POS_SAMPLE_H
#define POS_SAMPLE_H

#include <math/BoundingBox.h>
#include <math/miscfuncs.h>

using namespace alo;

struct PosSample {
	
	Vector3F _pos;
/// space-filling curve code
	int _key;
    float _uv[2];
	float _wei;
	
	void operator+=(const PosSample& b)
	{
		_pos += b._pos;
	}
	
	void operator/=(const float& b)
	{
		_pos /= b;
	}
	
	static std::string GetTypeStr() {
		return "possamp";
	}
	
};

struct SampleInterp {
	
	bool reject(const PosSample& asmp ) const {
		return false;
	}
	
	void interpolate(PosSample& asmp,
				const float* coord,
				const int* g) const {
	
	}
	
};


#endif
