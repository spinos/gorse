/*
 *  WorldGridLookupRule.h
 *  gorse
 *
 *  three layered distance lookup
 *  top bvh intersect local cells
 *  middle bvh intersect within a local cell
 *  low map distance and normal to instanced object
 *
 *  2019/5/4
 */

#ifndef ALO_GRD_WORLD_GRID_LOOK_UP_RULE_H
#define ALO_GRD_WORLD_GRID_LOOK_UP_RULE_H

#include <bvh/BVH.h>
#include <bvh/BVHRayTraverseRule.h>
#include <math/rayBox.h>

namespace alo {

namespace grd {

template<typename T>
struct WorldGridLookupResult {
	bvh::RayTraverseResult _rayBvh;
	typename T::LookupResultTyp _localParam;
	Vector3F _nml;
	float _t0;
};

template<typename T, typename Tc, typename Tp>
class WorldGridLookupRule {

	const T *m_grid;
	bvh::RayTraverseRule m_rayBvhRule;
	SimpleBuffer<Tp> m_cellRules;

public:

	typedef WorldGridLookupResult<Tp> LookupResultTyp;

	WorldGridLookupRule();

	void attach(const T *grid);
/// after attach
	template<typename Ti>
	void setPrimitiveRule(const Ti *x);
    void detach();
    bool isEmpty() const;

	bool lookup(LookupResultTyp &result, const float *rayData) const;

protected:

private:

	bool processLeaf(LookupResultTyp &result, const float *rayData) const;
/// among cells in a leaf
	int findClosestHit(bvh::RayTraverseResult &result, float *t, float *rayD) const;

	bool intersectPrimitive(LookupResultTyp &result, 
					const Tp &rule,
					const float *tLimit,
					const float *rayData) const;

};

template<typename T, typename Tc, typename Tp>
WorldGridLookupRule<T, Tc, Tp>::WorldGridLookupRule() : m_grid(nullptr)
{}

template<typename T, typename Tc, typename Tp>
void WorldGridLookupRule<T, Tc, Tp>::attach(const T *grid)
{
	m_grid = grid;
	m_rayBvhRule.attach(grid->boundingVolumeHierarchy());

	const int n = grid->numCells();
	m_cellRules.resetBuffer(n);
	for(int i=0;i<n;++i) {
		const Tc *ci = m_grid->c_cellPtr(i);
		m_cellRules[i].attach(ci->_grid);
	}

}

template<typename T, typename Tc, typename Tp>
template<typename Ti>
void WorldGridLookupRule<T, Tc, Tp>::setPrimitiveRule(const Ti *x)
{
	const int n = m_grid->numCells();
	for(int i=0;i<n;++i) {
		m_cellRules[i].setPrimitiveRule(x);
	}
}

template<typename T, typename Tc, typename Tp>
void WorldGridLookupRule<T, Tc, Tp>::detach()
{
	m_grid = nullptr;
}

template<typename T, typename Tc, typename Tp>
bool WorldGridLookupRule<T, Tc, Tp>::isEmpty() const
{ return m_grid == nullptr; }

template<typename T, typename Tc, typename Tp>
bool WorldGridLookupRule<T, Tc, Tp>::lookup(LookupResultTyp &result, const float *rayData) const
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

template<typename T, typename Tc, typename Tp>
bool WorldGridLookupRule<T, Tc, Tp>::processLeaf(LookupResultTyp &result, const float *rayData) const
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

		const Tp &cr = m_cellRules[firstHit];
		if(intersectPrimitive(result, cr, t, rayData)) return true;

/// advance to exit point
		tmpRay[6] = t[1] + 1e-4f;
		tmpRay[7] = bvhResult._t1;
	}

	return false;
}

template<typename T, typename Tc, typename Tp>
int WorldGridLookupRule<T, Tc, Tp>::findClosestHit(bvh::RayTraverseResult &result, float *t, float *rayD) const
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

template<typename T, typename Tc, typename Tp>
bool WorldGridLookupRule<T, Tc, Tp>::intersectPrimitive(LookupResultTyp &result, 
									const Tp &rule,
									const float *tLimit,
									const float *rayData) const
{
	Tp::LookupResultTyp &param = result._localParam;
	if(!rule.lookup(param, rayData)) return false;

	result._nml = param._nml;
	result._t0 = param._t0;
	return true;
}

} /// end of namepsace grd

} /// end of namespace alo

#endif
