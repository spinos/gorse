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

	bool isCloseTo(const Float2 &b, const float &thre=1e-4f) const {

		float d = b.x - x;
		if(d < -thre || d > thre) return false;
		d = b.y - y;
		return (d > -thre && d < thre);
	}
	
	float x, y;
};

}

#endif
