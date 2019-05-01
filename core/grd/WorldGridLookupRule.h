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

namespace alo {

namespace grd {

struct WorldGridLookupResult {
	bvh::RayTraverseResult _rayBvh;
};

template<typename T>
class WorldGridLookupRule {

	const T *m_grid;
	bvh::RayTraverseRule m_rayBvhRule;

public:

	WorldGridLookupRule();

	void attach(const T *grid);
    void detach();
    bool isEmpty() const;

	void lookup(WorldGridLookupResult &result, const float *rayData) const;

protected:


private:

};

template<typename T>
WorldGridLookupRule<T>::WorldGridLookupRule() : m_grid(nullptr)
{}

template<typename T>
void WorldGridLookupRule<T>::attach(const T *grid)
{
	m_grid = grid;
	m_rayBvhRule.attach(grid->boundingVolumeHierarchy());
}

template<typename T>
void WorldGridLookupRule<T>::detach()
{}

template<typename T>
bool WorldGridLookupRule<T>::isEmpty() const
{ return m_grid == nullptr; }

template<typename T>
void WorldGridLookupRule<T>::lookup(WorldGridLookupResult &result, const float *rayData) const
{
	m_rayBvhRule.begin(result._rayBvh, rayData);

	for(;;) {

		m_rayBvhRule.traverse(result._rayBvh);

		if(result._rayBvh._state == 0) break;

		std::cout << " leaf " << result._rayBvh._current
		<< " prim " << result._rayBvh._primBegin << ":" << result._rayBvh._primEnd;
	}
	
}

} /// end of namepsace grd

} /// end of namespace alo

#endif
