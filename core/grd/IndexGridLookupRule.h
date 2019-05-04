/*
 *  IndexGridLookupRule.h
 *  gorse
 *
 *  2019/5/4
 */

#ifndef ALO_INDEX_GRID_LOOKUP_RULE_H
#define ALO_INDEX_GRID_LOOKUP_RULE_H

#include <math/miscfuncs.h>
#include <math/pointBox.h>

namespace alo {

namespace grd {

struct IndexGridLookupResult {
    Int2 _instanceRange;
    int _instanceId;
    float _distance;

    IndexGridLookupResult() 
    {
    	_instanceRange.set(0,0);
    	_instanceId = -1;
    }

    bool isEmptySpace() const 
    { return _instanceRange.x >= _instanceRange.y; }

    bool hasInstanceRange() const
    { return _instanceRange.x < _instanceRange.y; }

};

template<typename T>
class IndexGridLookupRule {

	const T* m_grid;
	const float *m_originCellSize;
	float m_invCellSize;
	int m_dim[3];
	
public:

	IndexGridLookupRule();

	void attach(const T& grid);
    void detach();
    bool isEmpty() const;

	void lookup(IndexGridLookupResult &result, const float* p) const;

	const float *aabb() const;
/// i withing instance_range
	const int &index(int i) const;

	const int &numObjects() const;

	const float &cellSize() const;

protected:

private:

	void computeCellCoord(int* u, const float* p) const;
	int computeCellInd(const int* u) const;

};

template<typename T>
IndexGridLookupRule<T>::IndexGridLookupRule() : m_grid(nullptr)
{}

template<typename T>
void IndexGridLookupRule<T>::attach(const T& grid)
{
	m_grid = &grid;
	m_originCellSize = grid.originCellSize();
	m_invCellSize = 1.f / m_originCellSize[3];
	const int d = grid.resolution();
	m_dim[0] = d;
	m_dim[1] = d * d;
	m_dim[2] = m_dim[0] - 1;
}

template<typename T>
void IndexGridLookupRule<T>::detach()
{ m_grid = nullptr; }

template<typename T>
bool IndexGridLookupRule<T>::isEmpty() const
{ return m_grid == nullptr; }

template<typename T>
void IndexGridLookupRule<T>::computeCellCoord(int* u, const float* p) const
{
	u[0] = (p[0] - m_originCellSize[0]) * m_invCellSize;
	u[1] = (p[1] - m_originCellSize[1]) * m_invCellSize;
	u[2] = (p[2] - m_originCellSize[2]) * m_invCellSize;
	Clamp0b(u[0], m_dim[2]);
	Clamp0b(u[1], m_dim[2]);
	Clamp0b(u[2], m_dim[2]);
}

template<typename T>
int IndexGridLookupRule<T>::computeCellInd(const int* u) const
{ return (u[2]*m_dim[1] + u[1]*m_dim[0] + u[0]); }


template<typename T>
void IndexGridLookupRule<T>::lookup(IndexGridLookupResult &result, const float* p) const
{
	bool isEmptySpace = false;

    float toBox = 0.f;
    float q[3];
    memcpy(q, p, 12);
    if(isPointOutsideBox(q, m_grid->aabb() ) ) {
    	toBox = movePointOntoBox(q, m_grid->aabb() );
    	isEmptySpace = true;
    }

    if(isEmptySpace) {
    	result._instanceRange.set(0, 0);
    } else {
    	int u[3];
		computeCellCoord(u, q);
		const int offset = computeCellInd(u);
		result._instanceRange = m_grid->c_cell()[offset];
    }
	
	result._distance = m_grid->lookup(q) + toBox; 
}

template<typename T>
const float *IndexGridLookupRule<T>::aabb() const
{ return (const float *)&m_grid->fieldBox(); }

template<typename T>
const int &IndexGridLookupRule<T>::index(int i) const
{ return m_grid->c_indices()[i]; }

template<typename T>
const int &IndexGridLookupRule<T>::numObjects() const
{ return m_grid->numObjects(); }

template<typename T>
const float &IndexGridLookupRule<T>::cellSize() const
{ return m_grid->cellSize(); }

} /// end of namespace grd

} /// end of namespace alo

#endif

