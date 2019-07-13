/*
 *  SurfaceGeodesicSample.h
 *  aloe
 *
 *  surface position, normal, uv
 *  geodesic_distance and its gradient sample
 *
 *  2019/7/13
 */

#ifndef ALO_SURFACE_GEODESIC_SAMPLE_H
#define ALO_SURFACE_GEODESIC_SAMPLE_H

#include "SurfaceSample.h"

namespace alo {

struct SurfaceGeodesicSample {
	
	Vector3F _pos;
/// space-filling curve code
	int _key;
	Vector3F _nml;
    float _wei;
    Vector3F _grad;
    float _geod;
    float _uv[2];
	
	void operator+=(const SurfaceGeodesicSample& b)
	{
		_pos += b._pos;
        _nml += b._nml;
	}
	
	void operator/=(const float& b)
	{
		_pos /= b;
        _nml /= b;
	}
    
    SurfaceGeodesicSample operator+(const SurfaceGeodesicSample& b)
	{
        SurfaceGeodesicSample c;
		c._pos = _pos + b._pos;
        c._nml = _nml + b._nml;
        return c;
	}
    
    SurfaceGeodesicSample operator*(const float& b)
	{
        SurfaceGeodesicSample c;
		c._pos = _pos * b;
        c._nml = _nml * b;
        return c;
	}
	
	static std::string GetTypeStr() {
		return "surfgeodsamp";
	}
	
};

}

#endif
