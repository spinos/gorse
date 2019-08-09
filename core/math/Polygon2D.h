/*
 * Polygon2D.h
 * aloe
 *
 * closed segmented line in 2d
 * n points, n > 2
 * add in counterclockwise order
 *
 * 2019/8/10
 */

#ifndef ALO_MATH_POLYGON_2D_H
#define ALO_MATH_POLYGON_2D_H

#include "SimpleBuffer.h"
#include "Float2.h"

namespace alo {

class Polygon2D {

	SimpleBuffer<Float2> m_points;

public:

	Polygon2D();
	virtual ~Polygon2D();

	void reset();
	void operator<<(const Float2 &p);
	void finish();

	int numSegments() const;
	bool isValid() const;

	bool isPointInside(const Float2 &q) const;

protected:

private:

	void getSegment(Float2 &p0, Float2 &p1, const int &i) const;
	float angleBetween(const Float2 &p0, const Float2 &p1,
						const Float2 &q) const;

};

}

#endif
