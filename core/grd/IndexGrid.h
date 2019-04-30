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
	int m_numIndices;
    float m_aabb[6];

public:

	IndexGrid();
	virtual ~IndexGrid();
/// create grid, distance to very large
/// indices range to [0,0]
	void create(const int& x);
    
    void setAabb(const float *b);

    int *createIndices(int n);

    const int *c_indices() const;
    
    const float *aabb() const;

    const int &numIndices() const;
/// i-th cell range y > x
    bool isCellEmpty(int i) const;

    void verbose() const;
    
protected:
	int *indices();

private:

};

}

}

#endif