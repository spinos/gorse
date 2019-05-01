/*
 *  IndexGridLookupRule.h
 *  gorse
 *
 *  2019/4/29
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

	float m_originCellSize[4];
	float m_invCellSize[2];
	int m_maxNumStep;
	int m_dim[4];
	const T* m_grid;
	
public:

	IndexGridLookupRule();

	void setMaxNumStep(int x);
	const int &maxNumStep() const;
	
	void attach(const T& grid);
    void detach();
    bool isEmpty() const;

	void lookup(IndexGridLookupResult &result, const float* p) const;

protected:

private:

	void computeCellCoord(int* u, const float* p) const;
	int computeCellInd(const int* u) const;

};

template<typename T>
IndexGridLookupRule<T>::IndexGridLookupRule() : m_grid(nullptr),
m_maxNumStep(128)
{}

template<typename T>
void IndexGridLookupRule<T>::attach(const T& grid)
{
	grid.getOriginCellSize(m_originCellSize);
	m_invCellSize[0] = 1.f / m_originCellSize[3];
	const int d = grid.resolution();
	m_dim[0] = d;
	m_dim[1] = d * d;
	m_dim[2] = m_dim[0] - 1;
	m_grid = &grid;
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
	u[0] = (p[0] - m_originCellSize[0]) * m_invCellSize[0];
	u[1] = (p[1] - m_originCellSize[1]) * m_invCellSize[0];
	u[2] = (p[2] - m_originCellSize[2]) * m_invCellSize[0];
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
void IndexGridLookupRule<T>::setMaxNumStep(int x)
{ m_maxNumStep = x; }

template<typename T>
const int &IndexGridLookupRule<T>::maxNumStep() const
{ return m_maxNumStep; }

} /// end of namespace grd

} /// end of namespace alo

#endif

