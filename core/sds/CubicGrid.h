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

    void create(const int& x);

    void destroy();
    void copyCell(const Tc *b);

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
void CubicGrid<T, Tc>::create(const int& x)
{
	CubicField<T>::create(x);
	m_cells.resetBuffer(x * x * x);
}

template<typename T, typename Tc>
Tc *CubicGrid<T, Tc>::cell()
{ return m_cells.data(); }
    
template<typename T, typename Tc>
const Tc *CubicGrid<T, Tc>::c_cell() const
{ return m_cells.c_data(); }

template<typename T, typename Tc>
void CubicGrid<T, Tc>::destroy()
{
	CubicField<T>::destroy();
	m_cells.purgeBuffer();
}

template<typename T, typename Tc>
void CubicGrid<T, Tc>::copyCell(const Tc *b)
{ m_cells.copyFrom(b, numCells()); }

} /// end of namespace sds

} /// end of namespace alo

#endif
