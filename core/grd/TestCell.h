#ifndef ALO_GRD_TEST_CELL_H
#define ALO_GRD_TEST_CELL_H

#include <math/BoundingBox.h>

namespace alo {

namespace grd {

struct TestCell {

	BoundingBox _bbox;

	void setAabb(const float *x)
	{
		_bbox.setMin(x[0], x[1], x[2]);
		_bbox.setMax(x[3], x[4], x[5]);
	}

	const BoundingBox &aabb() const 
	{ return _bbox; }

	void getAabb(float *b) const
	{ memcpy(b, _bbox.data(), 24); }

};

}

}

#endif