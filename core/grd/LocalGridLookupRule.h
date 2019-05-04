/*
 *  LocalGridLookupRule.h
 *  gorse
 *
 *
 *  2019/5/4
 */

#ifndef ALO_GRD_LOCAL_GRID_LOOK_UP_RULE_H
#define ALO_GRD_LOCAL_GRID_LOOK_UP_RULE_H

#include <bvh/BVH.h>
#include <bvh/BVHRayTraverseRule.h>
#include "IndexGridLookupRule.h"
#include <math/rayBox.h>

namespace alo {

namespace grd {

struct LocalGridLookupResult {
	bvh::RayTraverseResult _rayBvh;
    Int2 _instanceRange;
    int _instanceId;
    Vector3F _nml;
    float _t0;
    float _q[3];
    int _u[3];

    LocalGridLookupResult() 
    {
    	_instanceRange.set(0,0);
    	_instanceId = -1;
    }

    bool isEmptySpace() const 
    { return _instanceRange.x >= _instanceRange.y; }

    bool hasInstanceRange() const
    { return _instanceRange.x < _instanceRange.y; }

};

template<typename T, typename Tp>
class LocalGridLookupRule {

	const T *m_grid;
	bvh::RayTraverseRule m_rayBvhRule;
	const Tp *m_primitiveRule;
	const float *m_originCellSize;
	float m_invCellSize;
	int m_dim[3];
	int m_maxNumStep;

public:

	LocalGridLookupRule();

	void setPrimitiveRule(const Tp *x);

	void attach(const T *grid);
    void detach();
    bool isEmpty() const;

	bool lookup(LocalGridLookupResult &result, const float *rayData) const;

	void setMaxNumStep(int x);

protected:

private:

	bool processLeaf(LocalGridLookupResult &result, const float *rayData) const;
/// among cells in a leaf
	int findClosestHit(bvh::RayTraverseResult &result, float *t, float *rayD) const;

	bool intersectPrimitive(LocalGridLookupResult &result, 
					const float *tLimit,
					const float *rayData) const;

	void computeCellCoord(int* u, const float* p) const;
	int computeCellInd(const int* u) const;

};

template<typename T, typename Tp>
LocalGridLookupRule<T, Tp>::LocalGridLookupRule() : m_grid(nullptr),
m_maxNumStep(128)
{}

template<typename T, typename Tp>
void LocalGridLookupRule<T, Tp>::setPrimitiveRule(const Tp *x)
{ m_primitiveRule = x; }

template<typename T, typename Tp>
void LocalGridLookupRule<T, Tp>::attach(const T *grid)
{
	m_grid = grid;
	m_rayBvhRule.attach(grid->boundingVolumeHierarchy());
	m_originCellSize = grid->originCellSize();
	m_invCellSize = 1.f / m_originCellSize[3];
	const int d = grid->resolution();
	m_dim[0] = d;
	m_dim[1] = d * d;
	m_dim[2] = m_dim[0] - 1;
}

template<typename T, typename Tp>
void LocalGridLookupRule<T, Tp>::detach()
{
	m_grid = nullptr;
}

template<typename T, typename Tp>
bool LocalGridLookupRule<T, Tp>::isEmpty() const
{ return m_grid == nullptr; }

template<typename T, typename Tp>
bool LocalGridLookupRule<T, Tp>::lookup(LocalGridLookupResult &result, const float *rayData) const
{
	bvh::RayTraverseResult &bvhResult = result._rayBvh;
	m_rayBvhRule.begin(bvhResult);

	for(;;) {

		m_rayBvhRule.traverse(bvhResult, rayData);

		if(m_rayBvhRule.end(bvhResult) ) break;

		if(processLeaf(result, rayData) ) return true;
		
	}
	
	return false;
}

template<typename T, typename Tp>
bool LocalGridLookupRule<T, Tp>::processLeaf(LocalGridLookupResult &result, const float *rayData) const
{
	bvh::RayTraverseResult &bvhResult = result._rayBvh;

	float tmpRay[8];
	memcpy(tmpRay, rayData, 32);
	tmpRay[6] = bvhResult._t0;
	tmpRay[7] = bvhResult._t1;
	float t[2];

	for(int i=bvhResult._primBegin;i<bvhResult._primEnd;++i) {

		int firstHit = findClosestHit(bvhResult, t, tmpRay);
		if(firstHit < 0) break;

		t[0] += 1e-4f;

		if(intersectPrimitive(result, t, rayData)) return true;

/// advance to exit point
		tmpRay[6] = t[1] + 1e-4f;
		tmpRay[7] = bvhResult._t1;
	}

	return false;
}

template<typename T, typename Tp>
int LocalGridLookupRule<T, Tp>::findClosestHit(bvh::RayTraverseResult &result, float *t, float *rayD) const
{
	const float t0 = rayD[6];
	const float t1 = rayD[7];

	int prim = -1;
	t[0] = 1e10f;
	for(int i=result._primBegin;i<result._primEnd;++i) {

		const float *primBox = (const float *)&m_grid->primitiveBox(i);

		if(rayAabbIntersect(rayD, primBox)) {
			if(t[0] > rayD[6]) {
/// closer
				t[0] = rayD[6];
				t[1] = rayD[7];
				prim = i;
			}
		}

		rayD[6] = t0;
		rayD[7] = t1;
		
	}

	return prim;
}

template<typename T, typename Tp>
void LocalGridLookupRule<T, Tp>::computeCellCoord(int* u, const float* p) const
{
	u[0] = (p[0] - m_originCellSize[0]) * m_invCellSize;
	u[1] = (p[1] - m_originCellSize[1]) * m_invCellSize;
	u[2] = (p[2] - m_originCellSize[2]) * m_invCellSize;
	Clamp0b(u[0], m_dim[2]);
	Clamp0b(u[1], m_dim[2]);
	Clamp0b(u[2], m_dim[2]);
}

template<typename T, typename Tp>
int LocalGridLookupRule<T, Tp>::computeCellInd(const int* u) const
{ return (u[2]*m_dim[1] + u[1]*m_dim[0] + u[0]); }

template<typename T, typename Tp>
bool LocalGridLookupRule<T, Tp>::intersectPrimitive(LocalGridLookupResult &result, 
									const float *tLimit,
									const float *rayData) const
{
	rayProgress(result._q, rayData, tLimit[0]);
	computeCellCoord(result._u, result._q);
	const int offset = computeCellInd(result._u);
	result._instanceRange = m_grid->c_cell()[offset];
	if(result.isEmptySpace())
		result._instanceRange.set(0, m_grid->numObjects());

	float tt = tLimit[0];

	for(int i=0;i<m_maxNumStep;++i) {
        
        rayProgress(result._q, rayData, tt);

        float d = m_primitiveRule->mapDistance(result._q, 
        	m_grid->c_indices(), result._instanceRange, 
        	result._instanceId);

        if(d < 1e-3f) break;

        if(d < tt * 1e-5f) break;

        tt += d * .7f;
     
        if(tt > tLimit[1]) return false;
    }

    m_primitiveRule->mapNormal(result._nml, result._q,
        	result._instanceId);

	result._t0 = tt;
	return true;
}

template<typename T, typename Tp>
void LocalGridLookupRule<T, Tp>::setMaxNumStep(int x)
{ m_maxNumStep = x; }

} /// end of namepsace grd

} /// end of namespace alo

#endif
