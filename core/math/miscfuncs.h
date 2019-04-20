/*
 *  miscfuncs.h
 *  aloe
 *
 *  Created by jian zhang on 12/18/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef ALO_MISC_FUNCS_H
#define ALO_MISC_FUNCS_H

#include <stdlib.h>
#include <math.h>

namespace alo {

inline char IsValueNearZero(const float & value)
{
	return (value < 1e-5f && value > -1e-5f);
}

inline void SwapValues(float &a, float &b)
{
	float t = a;
	a = b;
	b = t;
}

inline bool IsNan(float a) { return a != a; }

inline bool IsInf(float a) { return (a > 1e38 || a < -1e38); }

inline void Clamp01(float &v) {
	if(v < 0.f) v = 0.f;
	if(v > 1.f) v = 1.f;
}

inline void Clamp0255(int &x) {
	if(x < 0) x = 0;
	if(x > 255) x = 255;
}

inline void Clamp0b(int &x, int b) {
	if(x < 0) x = 0;
	if(x > b) x = b;
}

#ifdef WIN32
inline float log2f( float n )  
{  
    return logf( n ) * 3.32192809489f;  
}
#endif

#define PI 3.14159265358979323846
#define PIF 3.14159265358979f
static const float ONE_OVER_PIF = .318309886184f;
static const float ONE_OVER_2PIF = 0.15915494309f;
static const float TWO_PIF = 6.28318530718f;
#define HALFPIF 1.5707963268f
#define TWOPIF 6.28318530718f
#define ONEHALFPIF 4.71238898f
#define EPSILON 1e-7
#define GoldenRatio 1.618
#define ReGoldenRatio 0.382

inline bool CloseToZero(float a) {
    return (a < 1e-4 && a > -1e-4);
}

inline int GetSign(float d) {
    if(d> 0.f) return 1;
    if(d< 0.f) return -1;
    return 0;
}

inline float RandomF01()
{ return ((float)(rand() & 1023)) * 0.0009765625f; }

inline float RandomFn11()
{ return (RandomF01() - 0.5f) * 2.f; }

inline float RandomFlh(const float & lo, const float & hi)
{ return (lo + (hi - lo) * RandomF01() ); }

inline int DivideUp(const int& x, const int& a)
{
	int y = x / a;
	if(y*a < x) {
		y++;
	}
	return y;
}

template<typename T>
inline T Absolute(T const& a)
{
	return (a >= 0.0) ? a : -a;
}

template<typename T>
inline T DegreeToAngle(T const & a)
{
	return a * 0.0174532925;
}

template<typename T>
inline T AngleToDegree(T const & a)
{
	return a * 57.2957795;
}

template<typename T>
inline char IsNearZero(T const & a)
{
	if(a > EPSILON || a < -EPSILON) return 0;
	return 1;
}

template<typename T>
inline void SwapAB(T & a, T & b, T & c)
{ c = a; a = b; b = c; }

template<typename T>
inline void ClampInPlace(T & a, const T & lowLimit, const T & highLimit)
{
	if(a < lowLimit) a = lowLimit;
	if(a > highLimit) a = highLimit;
}

template<typename T>
inline T MixClamp01F(const T & a, const T & b, const float & w)
{ 
	if(w < 0.f) 
		return a;
		
	if(w > 1.f)
		return b;
		
	return a * (1.f - w) + b * w;
}

template<typename T>
inline T RemapF(const T & a, const T & b, 
				const float & low, const float & high,
				const float & v)
{ 
	float w = (v - low) / (high - low);
	return a * (1.f - w) + b * w;
}

template<typename T>
inline void SameSign(T & a, const T & b)
{
	if(a * b < 0)
		a = -a;
}

template<typename T>
inline T Clamp1(const T& x)
{ return x < 1.0 ? x : 1.0; }

template<typename T>
inline void Clamp_0_1023(T& x)
{
	if(x<0) x = 0;
	if(x>1023) x = 1023;
}

static const int TwelveHexagonEdges[12][2] = {
{0,1},
{2,3},
{4,5},
{6,7},

{0,2},
{1,3},
{4,6},
{5,7},

{0,4},
{1,5},
{2,6},
{3,7}
};

static const int SixNeighborOffsets[6][3] = {
{-1,0,0},
{ 1,0,0},
{0,-1,0},
{0, 1,0},
{0,0,-1},
{0,0, 1}
};

/// uv (phi, theta)
/// d direction in local space zenith is (0,0,1)
/// phi [0, 2PI]
/// theta [0, PI]
inline void ToSphericalCoord(float* uv, const float* d)
{
/// phi
	if(Absolute<float>(d[1]) < .9e-3f) {
	
		if(d[0] > 0.f) {
		if(d[1] < 0.f)
			uv[0] = TWOPIF;
		else
			uv[0] = 0.f;
		} else {
			uv[0] = PIF;
		}
			
	} else {
		uv[0] = acos(d[0] / sqrt(d[0] * d[0] + d[1] * d[1] ) );
		if(d[1] < 0.f)
			uv[0] = TWOPIF - uv[0];
			
	}
/// theta
	uv[1] = asin(d[2]) + HALFPIF;
}

}
#endif
