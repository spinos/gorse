/*
 *  WorldGridLookupRule.h
 *  gorse
 *
 *  three layered distance lookup
 *  top bvh intersection of cells
 *  middle instance index within a cell
 *  low distance to instanced object
 *
 *  2019/5/4
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

template<typename T, typename Tc, typename Ti>
class WorldGridLookupRule {

	const T *m_grid;
	const Ti *m_instancer;
	bvh::RayTraverseRule m_rayBvhRule;

	typedef IndexGridLookupRule<IndexGrid> CellIndexRuleTyp;
	SimpleBuffer<CellIndexRuleTyp> m_cellRules;

public:

	WorldGridLookupRule();

	void attach(const T *grid, const Ti *instancer);
    void detach();
    bool isEmpty() const;

	bool lookup(WorldGridLookupResult &result, const float *rayData) const;

protected:

private:

	bool processLeaf(WorldGridLookupResult &result, const float *rayData) const;
/// among cells in a leaf
	int findClosestHit(bvh::RayTraverseResult &result, float *t, float *rayD) const;

	bool intersectPrimitive(WorldGridLookupResult &result, 
					const CellIndexRuleTyp &rule,
					const float *tLimit,
					const float *rayData) const;

	float mapDistance(const float *q, IndexGridLookupResult &result, 
		const CellIndexRuleTyp &rule) const;

	void mapNormal(float *nml, const float *q, IndexGridLookupResult &result, 
		const CellIndexRuleTyp &rule) const;

	float findObjectClosestTo(const float *q, IndexGridLookupResult &result,
		const CellIndexRuleTyp &rule) const;

};

template<typename T, typename Tc, typename Ti>
WorldGridLookupRule<T, Tc, Ti>::WorldGridLookupRule() : m_grid(nullptr)
{}

template<typename T, typename Tc, typename Ti>
void WorldGridLookupRule<T, Tc, Ti>::attach(const T *grid, const Ti *instancer)
{
	m_grid = grid;
	m_rayBvhRule.attach(grid->boundingVolumeHierarchy());

	const int n = grid->numCells();
	m_cellRules.resetBuffer(n);
	for(int i=0;i<n;++i) {
		const Tc *ci = m_grid->c_cellPtr(i);
		m_cellRules[i].attach(*ci->_grid);
	}

	m_instancer = instancer;
}

template<typename T, typename Tc, typename Ti>
void WorldGridLookupRule<T, Tc, Ti>::detach()
{
	m_grid = nullptr;
}

template<typename T, typename Tc, typename Ti>
bool WorldGridLookupRule<T, Tc, Ti>::isEmpty() const
{ return m_grid == nullptr; }

template<typename T, typename Tc, typename Ti>
bool WorldGridLookupRule<T, Tc, Ti>::lookup(WorldGridLookupResult &result, const float *rayData) const
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

template<typename T, typename Tc, typename Ti>
bool WorldGridLookupRule<T, Tc, Ti>::processLeaf(WorldGridLookupResult &result, const float *rayData) const
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

		const CellIndexRuleTyp &cr = m_cellRules[firstHit];
		if(intersectPrimitive(result, cr, t, rayData)) return true;

/// advance to exit point
		tmpRay[6] = t[1] + 1e-4f;
		tmpRay[7] = bvhResult._t1;
	}

	return false;
}

template<typename T, typename Tc, typename Ti>
int WorldGridLookupRule<T, Tc, Ti>::findClosestHit(bvh::RayTraverseResult &result, float *t, float *rayD) const
{
	const float t0 = rayD[6];
	const float t1 = rayD[7];

	int prim = -1;
	t[0] = 1e10f;
	for(int i=result._primBegin;i<result._primEnd;++i) {

		const float *cellBox = m_cellRules[i].aabb();

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

template<typename T, typename Tc, typename Ti>
bool WorldGridLookupRule<T, Tc, Ti>::intersectPrimitive(WorldGridLookupResult &result, 
									const CellIndexRuleTyp &rule,
									const float *tLimit,
									const float *rayData) const
{
	IndexGridLookupResult &param = result._indGrd;

	float tt = tLimit[0];
	float q[3];
	for(int i=0;i<128;++i) {
		rayProgress(q, rayData, tt);

		float d = mapDistance(q, param, rule);

		if(d < 1e-3f) break;

        if(d < tt * 1e-5f) break;

        tt += d;

        if(tt > tLimit[1]) return false;
	}

	mapNormal((float *)&result._nml, q, param, rule);

	result._t0 = tt;
	return true;
}

template<typename T, typename Tc, typename Ti>
float WorldGridLookupRule<T, Tc, Ti>::mapDistance(const float *q, IndexGridLookupResult &result, const CellIndexRuleTyp &rule) const
{
	rule.lookup(result, q);
	if(result.isEmptySpace() ) {
		//&& result._distance > rule.cellSize() ) {
        return result._distance * .8f;
    }

    float md = findObjectClosestTo(q, result, rule);

    result._distance = md * .7f;
    m_instancer->limitStepSize(result._distance, result._instanceId);
    return result._distance;
}

template<typename T, typename Tc, typename Ti>
void WorldGridLookupRule<T, Tc, Ti>::mapNormal(float *nml, const float *q, IndexGridLookupResult &result, const CellIndexRuleTyp &rule) const
{
	if(result._instanceId < 0) findObjectClosestTo(q, result, rule);

	m_instancer->mapNormal(nml, q, result._instanceId);
}

template<typename T, typename Tc, typename Ti>
float WorldGridLookupRule<T, Tc, Ti>::findObjectClosestTo(const float *q, IndexGridLookupResult &result, const CellIndexRuleTyp &rule) const
{
	int begin = result._instanceRange.x;
	int end = result._instanceRange.y;
	if(begin >= end) {
		begin = 0;
		end = rule.numObjects();
	}

	float md = 1e10f;
    for(int i=begin;i<end;++i) {
    	const int &objI = rule.index(i);

    	float d = m_instancer->mapDistance(q, objI);
        if(md > d) {
            md = d;
            result._instanceId = objI;
        }

    }

    return md;
}

} /// end of namepsace grd

} /// end of namespace alo

#endif
