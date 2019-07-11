/*
 *  SparseCubicGrid.h
 *  gorse sds
 *
 *  cell indices of a dense 64^3 grid requires memory to store 262144 of T
 *  instead, to lookup sparse value of T
 *  index is mapped to cell by L2 tree
 *  assuming there are no more than half a million nonempty cells
 *
 *  2019/5/19
 */

#ifndef ALO_SDS_SPARSE_CUBIC_GRID_H
#define ALO_SDS_SPARSE_CUBIC_GRID_H

#include <math/BoundingBox.h>
#include <math/SimpleBuffer.h>
#include <math/Int2.h>
#include <sdb/L2Tree.h>

namespace alo {
    
namespace sds {

template<typename T>
class SparseCubicGrid {
/// limited num values
	sdb::L2Tree<int, int, 1024, 512> m_cellMap;
	typedef sdb::L3DataIterator<int, int, 512> IterTyp;
	SimpleBuffer<T> m_values;
/// (origin, cell_size)
	float m_originCellSize[4];
/// (M,M^2,M^3)
	int m_dim[3];
/// cubic
	BoundingBox m_fieldBox;

public:

	SparseCubicGrid();
	virtual ~SparseCubicGrid();

/// M
	void create(const int& x);
/// clear values and indices
	void reset();

	void createCellMap(const SimpleBuffer<Int2> &b);
	T *createValues(int n);
/// (origin, cell_size)
/// after creation
	void setOriginCellSize(const float* v);
/// cell[i] <- x
	T &addCell(const T &x, int i);
/// assuming cell[i] exists
	T &cell(int i);
	const T &c_cell(int i) const;
/// value[i]
	T &mappedCell(int i);
	const T &c_mappedCell(int i) const;

/// index to cell[i]
/// -1 if empty
	int hasCell(int i) const;
/// no index to cell[i]
    bool isCellEmpty(int i) const;

/// M
    const int &resolution() const;
/// M^3
	const int &numCells() const;
/// num values
	const int &numNonemptyCells() const;

	void getCellMap(SimpleBuffer<Int2> &b) const;
	const T *c_values() const;

	const float* originCellSize() const;
	const float& cellSize() const;

	const float *fieldAabb() const;
	const BoundingBox &fieldBox() const;
	void getCellBox(BoundingBox &b, int cellInd) const;

protected:

private:

};

template<typename T>
SparseCubicGrid<T>::SparseCubicGrid()
{}

template<typename T>
SparseCubicGrid<T>::~SparseCubicGrid()
{}

template<typename T>
void SparseCubicGrid<T>::create(const int& x)
{
	m_dim[0] = x;
	m_dim[1] = x * x;
	m_dim[2] = x * x * x;
	reset();
}

template<typename T>
void SparseCubicGrid<T>::setOriginCellSize(const float* v)
{
	m_originCellSize[0] = v[0];
	m_originCellSize[1] = v[1];
	m_originCellSize[2] = v[2];
	m_originCellSize[3] = v[3];
	
	m_fieldBox.setMin(v[0], v[1], v[2]);
    const int d = resolution();
    m_fieldBox.setMax(v[0] + v[3] * d, v[1] + v[3] * d, v[2] + v[3] * d);
}

template<typename T>
void SparseCubicGrid<T>::reset()
{
	m_cellMap.clear();
	m_values.purgeBuffer();
}

template<typename T>
void SparseCubicGrid<T>::createCellMap(const SimpleBuffer<Int2> &b)
{
	m_cellMap.clear();
	for(int i=0;i<b.count();++i) {
		const Int2 &bi = b[i];
		m_cellMap.insert(bi.x, bi.y);
	}
}

template<typename T>
T *SparseCubicGrid<T>::createValues(int n)
{
	m_values.resetBuffer(n);
	return m_values.data();
}

template<typename T>
T &SparseCubicGrid<T>::addCell(const T &x, int i)
{
	int *ind = m_cellMap.find(i);
	if(ind) return m_values[*ind];

	m_cellMap.insert(i, m_values.count());
	m_values << x;
	return m_values.last();
}

template<typename T>
T &SparseCubicGrid<T>::cell(int i)
{ return m_values[*m_cellMap.find(i)]; }

template<typename T>
const T &SparseCubicGrid<T>::c_cell(int i) const
{ 
	IterTyp it = m_cellMap.begin(i);
	return m_values[*it.second]; 
}

template<typename T>
T &SparseCubicGrid<T>::mappedCell(int i)
{ return m_values[i]; }

template<typename T>
const T &SparseCubicGrid<T>::c_mappedCell(int i) const
{ return m_values[i]; }

template<typename T>
int SparseCubicGrid<T>::hasCell(int i) const
{
	IterTyp it = m_cellMap.begin(i);
    if(it.first == i) return *it.second;
    return -1;
}

template<typename T>
bool SparseCubicGrid<T>::isCellEmpty(int i) const
{
	IterTyp it = m_cellMap.begin(i);
    return it.first != i;
}

template<typename T>
const int &SparseCubicGrid<T>::resolution() const
{ return m_dim[0]; }

template<typename T>
const int &SparseCubicGrid<T>::numCells() const
{ return m_dim[2]; }

template<typename T>
const int &SparseCubicGrid<T>::numNonemptyCells() const
{ return m_values.count(); }

template<typename T>
const float* SparseCubicGrid<T>::originCellSize() const
{ return m_originCellSize; }

template<typename T>
const float& SparseCubicGrid<T>::cellSize() const
{ return m_originCellSize[3]; }

template<typename T>
void SparseCubicGrid<T>::getCellMap(SimpleBuffer<Int2> &b) const
{
	IterTyp it = m_cellMap.begin(0);
	for(;!it.done();it.next()) {
      b << Int2(it.first, *it.second);
  	}
}

template<typename T>
const T *SparseCubicGrid<T>::c_values() const
{ return m_values.c_data(); }

template<typename T>
const float *SparseCubicGrid<T>::fieldAabb() const
{ return m_fieldBox.data(); }

template<typename T>
const BoundingBox &SparseCubicGrid<T>::fieldBox() const
{ return m_fieldBox; }

template<typename T>
void SparseCubicGrid<T>::getCellBox(BoundingBox &b, int cellInd) const
{
    const int &d = m_dim[0];
    const int &d2 = m_dim[1];
    int k = cellInd / d2;
    int j = (cellInd - k * d2) / d;
    int i = cellInd - k * d2 - j * d;
    const float h = cellSize();
    const float *ori = originCellSize();
    b.setMin(ori[0] + h * i,     ori[1] + h * j,     ori[2] + h *k);
    b.setMax(ori[0] + h * (i+1), ori[1] + h * (j+1), ori[2] + h * (k+1));
}

} /// end of sds

} /// end of alo

#endif
