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

	template<typename Ti, typename Tr, typename Tcb, typename Tcr>
	void addInstances(const Ti &instancer, Tr &gridRule, Tcb &cellBuilder, Tcr &cellRule);

protected:

private:
/// object to cell(s) by rule Tr
/// keep object-cell map for later build
	template<typename Tr>
	void mapCells(const float *b, const int &objI, Tr &rule);
/// build mapped cells by objects of T1, builder of Tb, and rule of Tr 
/// clear the object-cell map
	template<typename Ti, typename Tcb, typename Tcr>
	void buildCells(const Ti &instancer, Tcb &cellBuilder, Tcr &cellRule);

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
template<typename Ti, typename Tr, typename Tcb, typename Tcr>
void WorldGridBuilder<T, Tc>::addInstances(const Ti &instancer, Tr &gridRule, Tcb &cellBuilder, Tcr &cellRule)
{
	float b[6];
	int offset = 0;
	const int &n = instancer.numInstances();
	for(int i=0;i<n;++i) {
		instancer.getAabb(b, i);

		mapCells <Tr>(b, i, gridRule);

		offset++;
		if((offset & 1023) == 0) {
            offset = 0;
            buildCells<Ti, Tcb, Tcr>(instancer, cellBuilder, cellRule);
        }
	}

	if(offset > 0) {
		buildCells<Ti, Tcb, Tcr>(instancer, cellBuilder, cellRule);
	}
}

template<typename T, typename Tc>
template<typename Tr>
void WorldGridBuilder<T, Tc>::mapCells(const float *b, const int &objI, Tr &rule)
{
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
template<typename Ti, typename Tcb, typename Tcr>
void WorldGridBuilder<T, Tc>::buildCells(const Ti &instancer, Tcb &cellBuilder, Tcr &cellRule)
{
	const int ninst = m_objectCellMap.size();
	int nmc = 0;
	int preInd = -1;

	sdb::L3Node<sdb::Coord2, int, 1024> *block = m_objectCellMap.begin();
	while(block) {
		for (int i=0;i<block->count();++i) { 
        	
        	const sdb::Coord2 &ci = block->key(i);

			if(preInd < ci.y) {

				if(preInd > -1) {
					nmc++;
					cellBuilder.detach();
				}

				preInd = ci.y;

				Tc *cell = m_grid->findCell(ci.y);
				if(!cell) {
					std::cout << " ERROR WorldGridBuilder::buildCells cannot find cell " << ci.y;
					continue;
				}

				cellRule.setBBox((const float *)&cell->bbox());
				cellRule.setBoxRelativeBoundary(8.f);
				
				cellBuilder.attach(cell->_grid);
			}

			cellBuilder. template measureInstance<Ti, Tcr>(instancer, ci.x, cellRule);
		}
		block = m_objectCellMap.next(block);
	}

	if(preInd > -1) {
		nmc++;
		cellBuilder.detach();
	}

	m_objectCellMap.clear();

	std::cout << "\n INFO WorldGridBuilder built "<<ninst<<" instance in "<<nmc<<" cell ";

}

} /// end of namespace grd

} /// end of namespace alo

#endif