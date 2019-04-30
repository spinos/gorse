/*
 *  UniformGrid.h
 *  aloe
 *
 *  uniform field with cell values Tc
 *
 *  2019/4/30
 */

#ifndef ALO_UNIFORM_GRID_H
#define ALO_UNIFORM_GRID_H

#include "UniformField.h"

namespace alo {

namespace sds {

template<typename T, typename Tc>
class UniformGrid : public UniformField<T> {

	SimpleBuffer<Tc> m_cell;

public:

	UniformGrid();
	virtual ~UniformGrid();

	/// (M,N,P)
	void setResolution(const int* v);

	Tc* cell();
	const Tc* c_cell() const;

protected:

private:

};

template<typename T, typename Tc>
UniformGrid<T, Tc>::UniformGrid()
{}

template<typename T, typename Tc>
UniformGrid<T, Tc>::~UniformGrid()
{}

template<typename T, typename Tc>
void UniformGrid<T, Tc>::setResolution(const int* v)
{
	UniformField<T>::setResolution(v);
	m_cell.resetBuffer(v[0] * v[1] * v[2]);
}

template<typename T, typename Tc>
Tc* UniformGrid<T, Tc>::cell()
{ return m_cell.data(); }

template<typename T, typename Tc>
const Tc* UniformGrid<T, Tc>::c_cell() const
{ return m_cell.c_data(); }

} /// end of namespace sds

} /// end of namespace alo

#endif
