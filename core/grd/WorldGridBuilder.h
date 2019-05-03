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

/// (object, cell)
	sdb::L3Tree<sdb::Coord2, int, 2048, 512, 1024 > m_objectCellMap;
	WorldGrid<T, Tc> *m_grid;
	bool m_toRebuild;

public:

	WorldGridBuilder();
	virtual ~WorldGridBuilder();

	void attach(WorldGrid<T, Tc> *grid);
	void detach();

/// object of T1 to cell(s) by rule Tr
/// keep object-cell map for later build
	template<typename T1, typename Tr>
	void addObject(const T1 &obj, const int &objI, Tr &rule);
/// build mapped cells by objects of T1, builder of Tb, and rule of Tr 
/// clear the object-cell map
	template<typename T1, typename Tb, typename Tr>
	void buildCells(const T1 *objs, Tb &builder, Tr &rule);

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
void WorldGridBuilder<T, Tc>::addObject(const T1 &obj, const int &objI, Tr &rule)
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
			acell.create(1<<rule.P());
			acell.setAabb(cellBox);

			ci = m_grid->addCell(k[i], acell);

		}

		m_objectCellMap.insert(sdb::Coord2(objI, k[i]), 0);
	}

}

template<typename T, typename Tc>
template<typename T1, typename Tb, typename Tr>
void WorldGridBuilder<T, Tc>::buildCells(const T1 *objs, Tb &builder, Tr &rule)
{
	int preInd = -1;

	sdb::L3Node<sdb::Coord2, int, 1024> *block = m_objectCellMap.begin();
	while(block) {
		for (int i=0;i<block->count();++i) { 
        	
        	const sdb::Coord2 &ci = block->key(i);

			if(preInd < ci.y) {

				if(preInd > -1) builder.detach();

				preInd = ci.y;

				Tc *cell = m_grid->findCell(ci.y);
				if(!cell) {
					std::cout << " ERROR WorldGridBuilder::buildCells cannot find cell " << ci.y;
					continue;
				}

				rule.setBBox((const float *)&cell->aabb());
				rule.setBoxRelativeBoundary(2.f);
				
				builder.attach(cell->_grid);
			}

			builder. template measure<T1, Tr>(objs[ci.x], ci.x, rule);
		}
		block = m_objectCellMap.next(block);
	}

	if(preInd > -1) builder.detach();

	m_objectCellMap.clear();
}

} /// end of namespace grd

} /// end of namespace alo

#endif
