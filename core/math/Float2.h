#ifndef ALO_MATH_FLOAT_2_H
#define ALO_MATH_FLOAT_2_H

namespace alo {

struct Float2 {
	Float2(): x(0.f), y(0.f)
	{}
	
	Float2(float a, float b)
	: x(a), y(b)
	{}
	
	void set(float a, float b) { x = a; y = b; }
	
	float x, y;
};

}

#endif
