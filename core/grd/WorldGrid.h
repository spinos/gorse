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

/// in serial
	SimpleBuffer<const Tc *> m_cellPtr;
/// mapped
	sdb::L3Tree<T, Tc, 2048, 512, 1024 > m_cells;
	BVH *m_bvh;

public:

	WorldGrid();
	virtual ~WorldGrid();

	void clear();

	Tc *addCell(const T &k, Tc &x);

	Tc *findCell(const T &k);

	void buildBvh();

	const BVH *boundingVolumeHierarchy() const;
	const BoundingBox &primitiveBox(int i) const;
/// leafBegin <= i < leafEnd
	const Tc *c_cellPtr(int i) const;
/// of bvh root
	const BoundingBox &aabb() const;

	int numCells() const;
/// bvh num primitive
	int numNonEmptyCells() const;

protected:

private:

};

template<typename T, typename Tc>
WorldGrid<T, Tc>::WorldGrid() : m_bvh(nullptr)
{}

template<typename T, typename Tc>
WorldGrid<T, Tc>::~WorldGrid()
{
    if(m_bvh) delete m_bvh;
}

template<typename T, typename Tc>
void WorldGrid<T, Tc>::clear()
{
	m_cellPtr.purgeBuffer();

	sdb::L3Node<T, Tc, 1024> *block = m_cells.begin();
	while(block) {
		for (int i=0;i<block->count();++i) { 
        	
        	const Tc &ci = block->value(i);
			delete ci._grid;
            
		}
		block = m_cells.next(block);
	}
	m_cells.clear();
}

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
    if(m_bvh) delete m_bvh;
    m_bvh = new BVH;
	m_bvh->clear();

	m_cellPtr.resetBuffer(m_cells.size());

	BVHPrimitive ap;

	int offset = 0;

	sdb::L3Node<T, Tc, 1024> *block = m_cells.begin();
	while(block) {
		for (int i=0;i<block->count();++i) { 
        	
        	const Tc &ci = block->value(i);

        	if(ci.isEmpty()) continue;

			ap.setBBox(ci.bbox());
        	ap.setIndex(offset);

        	m_cellPtr[offset] = &ci;

        	offset++;

			m_bvh->addPrimitive(ap);
		}
		block = m_cells.next(block);
	}

	m_bvh->setRootLeaf();

	BVHSplit::InnerNumPrimitives = 16;
	BVHSplit::LeafNumPrimitives = 4;

    BVHBuilder::Build(m_bvh);

    std::cout << *m_bvh;

    int totalNCell = 0;
    int totalNNECell = 0;
    int totalNInd = 0;
    
    block = m_cells.begin();
	while(block) {
		for (int i=0;i<block->count();++i) { 
        	
        	const Tc &ci = block->value(i);

        	if(ci.isEmpty()) continue;

			ci._grid->buildBvh();

			totalNCell += ci._grid->numCells();
			totalNNECell += ci._grid->numNonemptyCells();
			totalNInd += ci._grid->numInstances();
            
		}
		block = m_cells.next(block);
	}

	std::cout << " " << totalNInd << " instance in " << totalNNECell << " local cell "
		 << 100.f - (float)totalNNECell / (float) totalNCell * 100.f << " percent empty ";
    
}

template<typename T, typename Tc>
const BVH *WorldGrid<T, Tc>::boundingVolumeHierarchy() const
{ return m_bvh; }

template<typename T, typename Tc>
const BoundingBox &WorldGrid<T, Tc>::primitiveBox(int i) const
{ return m_bvh->primitiveBox(i);}

template<typename T, typename Tc>
const Tc *WorldGrid<T, Tc>::c_cellPtr(int i) const
{ 
	const int &j = m_bvh->primitiveIndirection(i);
	return m_cellPtr[j]; 
}

template<typename T, typename Tc>
const BoundingBox &WorldGrid<T, Tc>::aabb() const
{ return m_bvh->aabb(); }

template<typename T, typename Tc>
int WorldGrid<T, Tc>::numCells() const
{ return m_cells.size(); }

template<typename T, typename Tc>
int WorldGrid<T, Tc>::numNonEmptyCells() const
{ 
	if(!m_bvh) return 0;
	return m_bvh->numPrimitives(); 
}

} /// end of namespace grd

} /// end of namespace alo

#endif
