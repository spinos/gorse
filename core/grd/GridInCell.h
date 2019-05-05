/*
 *  GridInCell.h
 *  gorse
 *
 *  access a T grid as a cell of a bigger grid
 *
 *  2019/5/4
 */

#ifndef ALO_GRD_GRID_IN_CELL_H
#define ALO_GRD_GRID_IN_CELL_H

namespace alo {

class BoundingBox;

namespace grd {

template<typename T>
struct GridInCell {

    T *_grid;

    void create(int x)
    {
        _grid = new T;
        _grid->create(x);
    }

	void setAabb(const float *x)
	{
        _grid->setAabb(x);
	}

	const BoundingBox &bbox() const 
	{ return _grid->fieldBox(); }

	void getAabb(float *b) const
	{ memcpy(b, _grid->fieldAabb(), 24); }

	bool isEmpty() const
	{ return _grid->isEmpty(); }

};

}

}

#endif
