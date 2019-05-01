/*
 *  WorldGridBuilder.h
 *  gorse
 *
 *
 *  2019/5/1
 */

#ifndef ALO_GRD_WORLD_GRID_BUILDER_H
#define ALO_GRD_WORLD_GRID_BUILDER_H

#include "WorldGrid.h"

namespace alo {

namespace grd {

template<typename T, typename Tc>
class WorldGridBuilder {

	WorldGrid<T, Tc> *m_grid;
	bool m_toRebuild;

public:

	WorldGridBuilder();
	virtual ~WorldGridBuilder();

	void attach(WorldGrid<T, Tc> *grid);
	void detach();

/// to cell(s)
	template<typename T1, typename Tr>
	void addObject(const T1 &obj, Tr &rule);

protected:

private:

};

template<typename T, typename Tc>
WorldGridBuilder<T, Tc>::WorldGridBuilder()
{}

template<typename T, typename Tc>
WorldGridBuilder<T, Tc>::~WorldGridBuilder()
{}

template<typename T, typename Tc>
void WorldGridBuilder<T, Tc>::attach(WorldGrid<T, Tc> *grid)
{ 
	m_grid = grid;
	m_toRebuild = false; 
}

template<typename T, typename Tc>
void WorldGridBuilder<T, Tc>::detach()
{
	if(m_toRebuild) m_grid->buildBvh();
}

template<typename T, typename Tc>
template<typename T1, typename Tr>
void WorldGridBuilder<T, Tc>::addObject(const T1 &obj, Tr &rule)
{
	float b[6];
	obj.getAabb(b);

	float cellBox[6];
	T k[8];
	int n = rule.calcCellCoords(k, b);

	for(int i=0;i<n;++i) {
		Tc *ci = m_grid->findCell(k[i]);
		if(!ci) {
			m_toRebuild = true;

			rule.calcCellAabb(cellBox, k[i]);

			Tc acell;
			acell.setAabb(cellBox);

			ci = m_grid->addCell(k[i], acell);

		} 
/// todo add object to cell

	}

}

} /// end of namespace grd

} /// end of namespace alo

#endif