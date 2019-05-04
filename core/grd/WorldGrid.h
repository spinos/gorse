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

/// indirection
	SimpleBuffer<int> m_cellInd;
/// in serial
	SimpleBuffer<const Tc *> m_cellPtr;
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
/// leafBegin <= i < leafEnd
	const Tc *c_cellPtr(int i) const;
/// of bvh root
	const BoundingBox &aabb() const;

	int numCells() const;

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
	m_bvh.clear();

	m_cellPtr.resetBuffer(m_cells.size());

	BVHPrimitive ap;

	int offset = 0;

	sdb::L3Node<T, Tc, 1024> *block = m_cells.begin();
	while(block) {
		for (int i=0;i<block->count();++i) { 
        	
        	const Tc &ci = block->value(i);

			ap.setBBox(ci.bbox());
        	ap.setIndex(offset);

        	m_cellPtr[offset] = &ci;

        	offset++;

			m_bvh.addPrimitive(ap);
		}
		block = m_cells.next(block);
	}

	m_bvh.setRootLeaf();

	BVHSplit::InnerNumPrimitives = 8;
	BVHSplit::LeafNumPrimitives = 2;

    BVHBuilder::Build(&m_bvh);

    m_cellInd.resetBuffer(m_cells.size());

    const BVHPrimitive *prims = m_bvh.c_primitives();
    const BVHNode *ns = m_bvh.c_nodes();
    const int &nn = m_bvh.numNodes();

	for(int i=0;i<nn;++i) {
		const BVHNode &ni = ns[i];
		if(ni.isLeaf()) {

			int b = ni.leafBegin();
			int e = ni.leafEnd();
			
			for(int j=b;j<e;++j) {
				m_cellInd[j] = prims[j].index();
			}
			
		}
	}

    std::cout << m_bvh;
}

template<typename T, typename Tc>
const BVH *WorldGrid<T, Tc>::boundingVolumeHierarchy() const
{ return &m_bvh; }

template<typename T, typename Tc>
const Tc *WorldGrid<T, Tc>::c_cellPtr(int i) const
{ return m_cellPtr[m_cellInd[i]]; }

template<typename T, typename Tc>
const BoundingBox &WorldGrid<T, Tc>::aabb() const
{ return m_bvh.aabb(); }

template<typename T, typename Tc>
int WorldGrid<T, Tc>::numCells() const
{ return m_cells.size(); }

} /// end of namespace grd

} /// end of namespace alo

#endif
