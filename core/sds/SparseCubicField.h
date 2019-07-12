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
	
public:

	SparseCubicField();
	virtual ~SparseCubicField();
	
/// M
	void create(const int& x);
/// (origin, cell_size)
/// after creation
	void setOriginCellSize(const float* v);
/// 8 corners of cell
	void addCell(const int *nodeCoord);
/// node indices
	void mapNodes();
/// i-th value <- x
    void setNodeValue(const int &i, T x);

	int numNodes() const;
    const int &resolution() const;
    const float *originCellSize() const;
    const T *c_values() const;

    typedef sdb::L3DataIterator<int, int, 512> NodeIterTyp;

    NodeIterTyp nodeBegin() const;
/// mapped index of node k
/// -1 if not exist
    int getNodeIndex(const int &k);

    void verbose() const;

    typedef T ValueTyp;
	
protected:

private:
/// clear values and indices
	void reset();

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
void SparseCubicField<T>::addCell(const int *nodeCoord)
{
	for(int i=0;i<8;++i) {
		int *ind = m_nodeMap.find(nodeCoord[i]);
		if(ind) continue;
		
		m_nodeMap.insert(nodeCoord[i], 0);
	}
}

template<typename T>
void SparseCubicField<T>::mapNodes()
{
	int count = 0;
	NodeIterTyp it = m_nodeMap.begin(0);
	for(;!it.done();it.next()) {
		*(it.second) = count;
      	count++;
  	}

  	m_values.resetBuffer(count);
}

template<typename T>
int SparseCubicField<T>::getNodeIndex(const int &k)
{
	int *ind = m_nodeMap.find(k);
	if(!ind) return -1;
	return *ind;
}

template<typename T>
int SparseCubicField<T>::numNodes() const
{ return m_nodeMap.size(); }

template<typename T>
const int &SparseCubicField<T>::resolution() const
{ return m_dim[0]; }

template<typename T>
const float *SparseCubicField<T>::originCellSize() const
{ return m_originCellSize; }

template<typename T>
const T *SparseCubicField<T>::c_values() const
{ return m_values.c_data(); }

template<typename T>
typename SparseCubicField<T>::NodeIterTyp SparseCubicField<T>::nodeBegin() const
{ return m_nodeMap.begin(0); }

template<typename T>
void SparseCubicField<T>::setNodeValue(const int &i, T x)
{ m_values[i] = x; }

template<typename T>
void SparseCubicField<T>::verbose() const
{
	std::cout << "\n SparseCubicField rez "<<resolution()
    <<" origin ("<<originCellSize()[0]<<", "<<originCellSize()[1]<<", "<<originCellSize()[2]
    <<") cell_size "<<originCellSize()[3]
    <<" n node " << numNodes();
}

}

}

#endif
