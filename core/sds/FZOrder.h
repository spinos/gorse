/*
 *  FZOrder.h
 *  aloe
 *
 *  Created by zhang on 18-2-17.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SDS_F_ORDER_H
#define ALO_SDS_F_ORDER_H

#include <math/morton3.h>

namespace alo {

namespace sds {

struct FZOrderCurve {
	
	float _origin[3];
/// span / 1024
	float _h;
	float _oneoverh;
	int _range[2];
    
/// (center, half_span)
    void setCoord(const float& centerx,
					const float& centery,
					const float& centerz,
					const float& halfspan) 
    {
        _h = halfspan / 512.f;
		_oneoverh = 512.f / halfspan;
        _origin[0] = centerx - halfspan;
		_origin[1] = centery - halfspan;
		_origin[2] = centerz - halfspan;
		
    }
    
    void setCoord(const float* centerHalfspan) 
    { 
        setCoord(centerHalfspan[0],
                centerHalfspan[1],
                centerHalfspan[2],
                centerHalfspan[3]); 
    }
	
	void setRange(int xlow, int ylow, int zlow,
				int xhigh, int yhigh, int zhigh)
	{
		_range[0] = encodeMorton3D(xlow, ylow, zlow);
		_range[1] = encodeMorton3D(xhigh, yhigh, zhigh);
	}
	
	void setOriginSpan(const float& originx,
					const float& originy,
					const float& originz,
					const float& span) 
	{
		_origin[0] = originx;
		_origin[1] = originy;
		_origin[2] = originz;
		_h = span / 1024.f;
		_oneoverh = 1024.f / span;
	}
	
	int computeKey(const float* p) const
	{
		int x = (p[0] - _origin[0]) * _oneoverh;
		int y = (p[1] - _origin[1]) * _oneoverh;
		int z = (p[2] - _origin[2]) * _oneoverh;
		return encodeMorton3D(x, y, z);
	}
	
	int computeKey(int x, int y, int z) const
	{
		return encodeMorton3D(x, y, z);
	}
/// level [1,9]    
    int computeKeyAtLevel(const float* p, const int& level) const
	{
        int x = (p[0] - _origin[0]) * _oneoverh;
		int y = (p[1] - _origin[1]) * _oneoverh;
		int z = (p[2] - _origin[2]) * _oneoverh;
        const int l = 10 - level;
        x = (x>>l)<<l;
        y = (y>>l)<<l;
        z = (z>>l)<<l;
		return encodeMorton3D(x, y, z);
	}
	
	int computeKeyAtLevel(const int* u, const int& level) const
	{
        const int l = 10 - level;
        int x = (u[0]>>l)<<l;
        int y = (u[1]>>l)<<l;
        int z = (u[2]>>l)<<l;
		return encodeMorton3D(x, y, z);
	}
	
	void decodeKey(int* u, const int& k) const
	{
		decodeMortoni(u, k);
	}
	
	void decodeKey(float* p, const int& k) const
	{
		int u[3];
		decodeMortoni(u, k);
		p[0] = _origin[0] + _h * u[0];
		p[1] = _origin[1] + _h * u[1];
		p[2] = _origin[2] + _h * u[2];
	}
    
    void getBox(float* centerHalfSpan) const
	{
        const float hs = _h * 512.f;
        centerHalfSpan[0] = _origin[0] + hs;
		centerHalfSpan[1] = _origin[1] + hs;
		centerHalfSpan[2] = _origin[2] + hs;
		centerHalfSpan[3] = hs;
		
	}
    
    void getBoxAtLevel(float* centerHalfSpan, const float* p, const int& level) const
    {
        if(level < 1) {
            getBox(centerHalfSpan);
            return;
        }
        
        int x = (p[0] - _origin[0]) * _oneoverh;
		int y = (p[1] - _origin[1]) * _oneoverh;
		int z = (p[2] - _origin[2]) * _oneoverh;
        const int l = 10 - level;
        x = (x>>l)<<l;
        y = (y>>l)<<l;
        z = (z>>l)<<l;
        
        int w = 512>>level;
        centerHalfSpan[0] = _origin[0] + _h * (x + w);
        centerHalfSpan[1] = _origin[1] + _h * (y + w);
        centerHalfSpan[2] = _origin[2] + _h * (z + w);
        centerHalfSpan[3] = _h * w;
    }

    int computeLevelBySpan(const float& x) const 
    {
        float s = x * _oneoverh;
        int l = 0;
        while((int)s<1024) {
            s *= 2;
            l++;
        }
        return l;
    }
	
	int computeLevelBySpani(const int& x) const 
    {
        float s = x;
        int l = 0;
        while((int)s<1024) {
            s *= 2;
            l++;
        }
        return l;
    }
    
};

}

}

#endif
