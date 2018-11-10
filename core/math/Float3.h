#ifndef ALO_MATH_FLOAT_3_H
#define ALO_MATH_FLOAT_3_H

namespace alo {

struct Float3 {
	Float3(): x(0.f), y(0.f), z(0.f)
	{}
	
	Float3(float a, float b, float c)
	: x(a), y(b), z(c)
	{}
	
	void set(float a, float b, float c) { x = a; y = b; z=c; }
	
	float x, y, z;
};

}

#endif
