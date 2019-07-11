/*
 *  SparseCubicField.h
 *  gorse sds
 *
 *  value stored in cell corners
 *
 *  2019/7/11
 */
 
#ifndef ALO_SDS_SPARSE_CUBIC_FIELD_H
#define ALO_SDS_SPARSE_CUBIC_FIELD_H

#include <math/BoundingBox.h>
#include <math/SimpleBuffer.h>
#include <math/Int2.h>
#include <sdb/L2Tree.h>

namespace alo {
    
namespace sds {

template<typename T>
class SparseCubicField {
	
	SimpleBuffer<T> m_values;
/// (origin, cell_size)
	float m_originCellSize[4];
/// (M,M^2,M^3)
	int m_dim[3];
/// cubic
	BoundingBox m_fieldBox;
/// limited num values
	sdb::L2Tree<int, int, 1024, 512> m_nodeMap;
	typedef sdb::L3DataIterator<int, int, 512> IterTyp;
	
public:

	SparseCubicField();
	virtual ~SparseCubicField();
	
/// M
	void create(const int& x);
/// clear values and indices
	void reset();
/// (origin, cell_size)
/// after creation
	void setOriginCellSize(const float* v);
/// 8 corners of cell
	void addCell(const int *nodeCoord, const T *x);
/// M
    const int &resolution() const;
	
protected:

private:
	
};

template<typename T>
SparseCubicField<T>::SparseCubicField()
{}

template<typename T>
SparseCubicField<T>::~SparseCubicField()
{}

template<typename T>
void SparseCubicField<T>::create(const int& x)
{
	m_dim[0] = x;
	m_dim[1] = x * x;
	m_dim[2] = x * x * x;
	reset();
}

template<typename T>
void SparseCubicField<T>::reset()
{
	m_nodeMap.clear();
	m_values.purgeBuffer();
}

template<typename T>
void SparseCubicField<T>::setOriginCellSize(const float* v)
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
void SparseCubicField<T>::addCell(const int *nodeCoord, const T &x)
{
	for(int i=0;i<8;++i) {
		int *ind = m_nodeMap.find(nodeCoord[i]);
		if(ind) continue;
		
		m_cellMap.insert(nodeCoord[i], m_values.count());
		m_values << x;
	}
}

template<typename T>
const int &SparseCubicField<T>::resolution() const
{ return m_dim[0]; }

}

}

#endif
