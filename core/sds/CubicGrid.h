/*
 *  CubicGrid.h
 *  gorse
 *
 *  cubic field of node type T with cell type Tc
 *
 *  2019/4/25
 */
 
#ifndef ALO_CUBIC_GRID_H
#define ALO_CUBIC_GRID_H

#include "CubicField.h"

namespace alo {

namespace sds {

template<typename T, typename Tc>
class CubicGrid : public CubicField<T> {
    
    SimpleBuffer<Tc> m_cells;

public:
    
    CubicGrid();
    virtual ~CubicGrid();

    void setResolution(const int& x);

    Tc *cell();
    const Tc *c_cell() const;
    
protected:

private:

};

template<typename T, typename Tc>
CubicGrid<T, Tc>::CubicGrid()
{}

template<typename T, typename Tc>
CubicGrid<T, Tc>::~CubicGrid()
{}

template<typename T, typename Tc>
void CubicGrid<T, Tc>::setResolution(const int& x)
{
	CubicField<T>::setResolution(x);
	m_cells.resetBuffer(x * x * x);
}

template<typename T, typename Tc>
Tc *CubicGrid<T, Tc>::cell()
{ return m_cells.data(); }
    
template<typename T, typename Tc>
const Tc *CubicGrid<T, Tc>::c_cell() const
{ return m_cells.c_data(); }

} /// end of namespace sds

} /// end of namespace alo

#endif

