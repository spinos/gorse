/*
 *  IndexGrid.h
 *  gorse
 *
 *  cubic field stores distance to object bound as node values
 *  cell values stores index range to object inside
 *  first n_obj indices are object ids
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
    int m_numObjects;
    float m_aabb[6];

public:

	IndexGrid();
	virtual ~IndexGrid();
/// create grid by resolution x, initial distance to very large
/// indices range to [0,0]
	void create(const int& x);
    
    void setAabb(const float *b);
/// unique object id count
    void setNumObjects(int x);

    int *createIndices(int n);

    const int *c_indices() const;
    
    const float *aabb() const;

    const int &numIndices() const;
    const int &numObjects() const;
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