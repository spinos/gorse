/*
 *  IndexGrid.h
 *  gorse
 *
 *  cubic field stores distance to object bound as node values
 *  cell values stores index range to object inside
 *
 *  2019/4/28
 */

#ifndef ALO_INDEX_GRID_H
#define ALO_INDEX_GRID_H

#include <sds/CubicGrid.h>
#include <math/Int2.h>

namespace alo {
    
namespace grd {

class IndexGrid : public sds::CubicGrid<float, Int2> {

typedef sds::CubicGrid<float, Int2> PGridTyp;

	SimpleBuffer<int> m_indices;

public:

	IndexGrid();
	virtual ~IndexGrid();
/// create grid, distance to very large
/// indices range to [0,0]
	void setResolution(const int& x);
    
    void setAabb(const float *b);

protected:

private:

};

}

}

#endif