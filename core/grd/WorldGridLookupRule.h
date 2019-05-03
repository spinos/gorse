/*
 *  WorldGridLookupRule.h
 *  gorse
 *
 *
 *  2019/5/2
 */

#ifndef ALO_GRD_WORLD_GRID_LOOK_UP_RULE_H
#define ALO_GRD_WORLD_GRID_LOOK_UP_RULE_H

#include <bvh/BVH.h>
#include <bvh/BVHRayTraverseRule.h>
#include "IndexGridLookupRule.h"
#include <math/rayBox.h>

namespace alo {

namespace grd {

struct WorldGridLookupResult {
	bvh::RayTraverseResult _rayBvh;
	IndexGridLookupResult _indGrd;
	Vector3F _nml;
	float _t0;
};

template<typename T, typename Tc>
class WorldGridLookupRule {

	const T *m_grid;
	bvh::RayTraverseRule m_rayBvhRule;
/// per-cell?
	IndexGridLookupRule<Tc> m_cellRule;

public:

	WorldGridLookupRule();

	void attach(const T *grid);
    void detach();
    bool isEmpty() const;

	bool lookup(WorldGridLookupResult &result, const float *rayData) const;

protected:
/// q <- ray.o + ray.d * t
	static void rayTravel(float *q, const float *ray, const float &t);

private:

	bool processLeaf(WorldGridLookupResult &result, const float *rayData) const;
/// among cells in a leaf
	int findClosestHit(bvh::RayTraverseResult &result, float *t, float *rayD) const;

	bool intersectPrimitive(WorldGridLookupResult &result, 
					const Tc *cell,
					const float *t,
					const float *rayData) const;

};

template<typename T, typename Tc>
WorldGridLookupRule<T, Tc>::WorldGridLookupRule() : m_grid(nullptr)
{}

template<typename T, typename Tc>
void WorldGridLookupRule<T, Tc>::attach(const T *grid)
{
	m_grid = grid;
	m_rayBvhRule.attach(grid->boundingVolumeHierarchy());
}

template<typename T, typename Tc>
void WorldGridLookupRule<T, Tc>::detach()
{
	m_grid = nullptr;
}

template<typename T, typename Tc>
bool WorldGridLookupRule<T, Tc>::isEmpty() const
{ return m_grid == nullptr; }

template<typename T, typename Tc>
bool WorldGridLookupRule<T, Tc>::lookup(WorldGridLookupResult &result, const float *rayData) const
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

template<typename T, typename Tc>
bool WorldGridLookupRule<T, Tc>::processLeaf(WorldGridLookupResult &result, const float *rayData) const
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

		const Tc *ci = m_grid->c_cellPtr(firstHit);
		if(intersectPrimitive(result, ci, t, rayData)) {
			result._t0 = t[0];
			return true;
		}

/// advance to exit point
		tmpRay[6] = t[1] + 1e-3f;
		tmpRay[7] = bvhResult._t1;
	}

	return false;
}

template<typename T, typename Tc>
int WorldGridLookupRule<T, Tc>::findClosestHit(bvh::RayTraverseResult &result, float *t, float *rayD) const
{
	const float t0 = rayD[6];
	const float t1 = rayD[7];

	int prim = -1;
	t[0] = 1e10f;
	for(int i=result._primBegin;i<result._primEnd;++i) {

		const Tc *ci = m_grid->c_cellPtr(i);

		const float *cellBox = (const float *)&ci->aabb();

		if(rayAabbIntersect(rayD, cellBox)) {
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

template<typename T, typename Tc>
bool WorldGridLookupRule<T, Tc>::intersectPrimitive(WorldGridLookupResult &result, 
									const Tc *cell,
									const float *t,
									const float *rayData) const
{
/// advance to entry point
	float q[3];
	rayProgress(q, rayData, t[0]);

	const float *cellBox = (const float *)&cell->aabb();

	normalOnBox((float *)&result._nml, q, cellBox);
	return true;
}

} /// end of namepsace grd

} /// end of namespace alo

#endif
