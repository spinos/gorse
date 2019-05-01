/*
 *  WorldGrid.h
 *  gorse
 *
 *  top-level grid of unknown bound
 *  map cell type Tc by key type T
 *  ray-intersection using bvh  
 *
 *  2019/5/1
 */

#ifndef ALO_GRD_WORLD_GRID_H
#define ALO_GRD_WORLD_GRID_H

#include <bvh/BVH.h>
#include <bvh/BVHBuilder.h>
#include <bvh/BVHSplit.h>
#include <bvh/BVHPrimitive.h>
#include <sdb/L3Tree.h>
#include <math/SimpleBuffer.h>

namespace alo {

namespace grd {

template<typename T, typename Tc>
class WorldGrid {

	typedef Tc * CellPtrTyp;
/// in serial
	SimpleBuffer<CellPtrTyp> m_pcells;
/// mapped
	sdb::L3Tree<T, Tc, 2048, 512, 1024 > m_cells;
	BVH m_bvh;

public:

	WorldGrid();
	virtual ~WorldGrid();

	Tc *addCell(const T &k, Tc &x);

	Tc *findCell(const T &k);

	void buildBvh();

	const BVH *boundingVolumeHierarchy() const;

protected:

private:

};

template<typename T, typename Tc>
WorldGrid<T, Tc>::WorldGrid()
{}

template<typename T, typename Tc>
WorldGrid<T, Tc>::~WorldGrid()
{}

template<typename T, typename Tc>
Tc *WorldGrid<T, Tc>::findCell(const T &k)
{
	return m_cells.find(k);
}

template<typename T, typename Tc>
Tc *WorldGrid<T, Tc>::addCell(const T &k, Tc &x)
{ 
	return m_cells.insert(k, x); 
}

template<typename T, typename Tc>
void WorldGrid<T, Tc>::buildBvh()
{
	m_pcells.resetBuffer(m_cells.size());

	m_bvh.clear();

	BVHPrimitive ap;

	int offset = 0;

	sdb::L3Node<T, Tc, 1024> *block = m_cells.begin();
	while(block) {
		for (int i=0;i<block->count();++i) { 
        	
        	Tc &ci = block->value(i);

			ap.setAABB(ci.aabb());
        	ap.setIndex(offset);

        	m_pcells[offset] = &ci;

        	offset++;

			m_bvh.addPrimitive(ap);
		}
		block = m_cells.next(block);
	}

	m_bvh.setRootLeaf();

	BVHSplit::InnerNumPrimitives = 8;
	BVHSplit::LeafNumPrimitives = 2;

    BVHBuilder::Build(&m_bvh);

    std::cout << m_bvh;
}

template<typename T, typename Tc>
const BVH *WorldGrid<T, Tc>::boundingVolumeHierarchy() const
{ return &m_bvh; }

} /// end of namespace grd

} /// end of namespace alo

#endif
