/*
 *  SurfaceSample.h
 *  aloe
 *
 *  surface position, normal, uv sample
 *
 *  Created by jian zhang on 4/19/19.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SURFACE_SAMPLE_H
#define ALO_SURFACE_SAMPLE_H

#include <math/Vector3F.h>

namespace alo {

struct SurfaceSample {
	
	Vector3F _pos;
/// space-filling curve code
	int _key;
	Vector3F _nml;
    float _uv[2];
	float _wei;
	
	void operator+=(const SurfaceSample& b)
	{
		_pos += b._pos;
        _nml += b._nml;
	}
	
	void operator/=(const float& b)
	{
		_pos /= b;
        _nml /= b;
	}
    
    SurfaceSample operator+(const SurfaceSample& b)
	{
        SurfaceSample c;
		c._pos = _pos + b._pos;
        c._nml = _nml + b._nml;
        return c;
	}
    
    SurfaceSample operator*(const float& b)
	{
        SurfaceSample c;
		c._pos = _pos * b;
        c._nml = _nml * b;
        return c;
	}
	
	static std::string GetTypeStr() {
		return "surfsamp";
	}
	
};

struct SampleInterp {
	
	bool reject(const SurfaceSample& asmp ) const {
		return false;
	}
	
	void interpolate(SurfaceSample& asmp,
				const float* coord,
				const int* g) const {
	
	}
	
};

}

#endif
