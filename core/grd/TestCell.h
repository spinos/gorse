#ifndef ALO_GRD_TEST_CELL_H
#define ALO_GRD_TEST_CELL_H

#include <math/BoundingBox.h>
#include <math/pointBox.h>
#include <math/boxBox.h>

namespace alo {

namespace grd {

template<typename T>
struct TestCell {

    T *_grid;
	BoundingBox _bbox;
    
    void create(int x)
    {
        _grid = new T;
        _grid->create(x);
    }

	void setAabb(const float *x)
	{
		_bbox.setMin(x[0], x[1], x[2]);
		_bbox.setMax(x[3], x[4], x[5]);
        _grid->setAabb(x);
	}

	const BoundingBox &aabb() const 
	{ return _bbox; }

	void getAabb(float *b) const
	{ memcpy(b, _bbox.data(), 24); }

};

struct TestBox {

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
    
    float mapDistance(const float *p) const 
    {
        const float *b = (const float *)&_bbox;
        if(isPointOutsideBox(p, b))
            return distanceOutsidePointToBox(p, b);

        return -distanceInsidePointToBox(p, b);
    }
    
    bool intersectBox(const float* b) const
    {
        const float *cb = (const float *)&_bbox;
        return boxIntersectBox(cb, b);
    }

};

}

}

#endif