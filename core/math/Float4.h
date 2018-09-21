/*
 *  Float4.h
 *  aloe
 *
 *  Created by zhang on 18-2-21.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_MATH_FLOAT4_H
#define ALO_MATH_FLOAT4_H

namespace alo {

struct Float4 {
	Float4(): x(0.f), y(0.f), z(0.f), w(0.f)
	{}
	
	Float4(float a, float b, float c, float d)
	: x(a), y(b), z(c), w(d)
	{}
	
	void set(float a, float b, float c, float d)
	{ x = a; y = b; z = c; w = d; }
	
	Float4 operator * ( const float& scale ) const
	{
		return Float4(
		x * scale,
		y * scale,
		z * scale,
		w * scale);
	}
	
	Float4 operator + ( const Float4 & b ) const
	{
		return Float4(
		x + b.x,
		y + b.y,
		z + b.z,
		w + b.w);
	}
	
	float dot(const Float4 & b) const
	{
		return x * b.x + y * b.y + z * b.z + w * b.w;
	}
	
	float x, y, z, w;
};

}

#endif
