/*
 *  IndexGridBuilder.h
 *  gorse
 *
 *  P is level of grid
 *  2^(P*3) cells
 *
 *  2019/4/26
 */

#ifndef ALO_INDEX_GRID_BUILDER_H
#define ALO_INDEX_GRID_BUILDER_H

#include "IndexGrid.h"
#include <sdb/Types.h>
#include <sdb/L3Tree.h>

namespace alo {
    
namespace grd {

template<int P>
class IndexGridBuilder {

	IndexGrid *m_grid;
/// (object, cell) as key
	sdb::L3Tree<sdb::Coord2, int, 2048, 512, 1024 > m_objectCellMap;

public:

	IndexGridBuilder();
	~IndexGridBuilder();

	void attach(IndexGrid *grid);
/// finish grid
	void detach();
/// distance to sample
/// index in cell
/// T is sample type
/// Tr is build rule
	template<typename T, typename Tr>
	void measure(const T &sample, int objI, Tr &rule);

	template<typename Ti, typename Tr>
	void measureInstance(const Ti &instancer, int instanceI, Tr &rule);

protected:

private:
/// extract existing indices in grid
	void recordIndices(IndexGrid *grid);

	struct CellKH {
		int _key;
		int _span;
	};
};

template<int P>
IndexGridBuilder<P>::IndexGridBuilder()
{}

template<int P>
IndexGridBuilder<P>::~IndexGridBuilder()
{}

template<int P>
void IndexGridBuilder<P>::attach(IndexGrid *grid)
{
	m_grid = grid;
	if(grid->numIndices()) recordIndices(grid);
}

template<int P>
void IndexGridBuilder<P>::detach()
{
	sdb::L3Tree<int, int, 2048, 512, 1024 > allObjs;
	sdb::L3Node<sdb::Coord2, int, 1024> *block = m_objectCellMap.begin();
	while(block) {
		for (int i=0;i<block->count();++i) { 
			const sdb::Coord2 &ci = block->key(i);
			allObjs.insert(ci.x, 0);

		}
		block = m_objectCellMap.next(block);
	}

	const int nObjs = allObjs.size();
	m_grid->setNumObjects(nObjs);

	const int numInstances = m_objectCellMap.size();
	int *inds = m_grid->createIndices(nObjs + numInstances);

	int offset = 0;

	sdb::L3Node<int, int, 1024> *oblock = allObjs.begin();
	while(oblock) {
		for (int i=0;i<oblock->count();++i) { 
			inds[offset] = oblock->key(i);
			offset++;
		}
		oblock = allObjs.next(oblock);
	}
	
	int countNonEmptyCells = 0;
	int preCell = -1;
	
	block = m_objectCellMap.begin();
	while(block) {
		for (int i=0;i<block->count();++i) { 
			const sdb::Coord2 &ci = block->key(i);

			inds[offset] = ci.x;
			
			Int2 &cellRange = m_grid->cell()[ci.y];

			if(preCell != ci.y) {
				preCell = ci.y;
				countNonEmptyCells++;

				cellRange.x = offset;
			}

			offset++;
			cellRange.y = offset;
		}
		block = m_objectCellMap.next(block);
	}

	m_objectCellMap.clear();

}

template<int P>
template<typename T, typename Tr>
void IndexGridBuilder<P>::measure(const T &sample, int objI, Tr &rule)
{
    const int d = 1<<P;
    const int n = (d+1)*(d+1)*(d+1);
	int* ks = new int[n];
	rule.getLevelGrid(ks, 0, 0, P);
    
    float *distOnGrid = m_grid->value();
    float pos[3];
    for(int i=0;i<n;++i) {
        rule.computePosition(pos, ks[i]);
        float dist = sample.mapDistance(pos);
/// shorten the distance
        if(distOnGrid[i] > dist) distOnGrid[i] = dist;

    }
    
    delete[] ks;
    
/// store objects in cell 
	const int nc = d * d * d; 
	CellKH *cs = new CellKH[6 * nc];  
	rule. template getLevelCells<CellKH>(cs, 0, 0, P);  

	float b[6];
    for(int i=0;i<nc;++i) {

    	rule. template getCellBox<CellKH>(b, cs[i]);

    	rule.expandBox(b);

    	if(sample.intersectBox(b)) {
    		m_objectCellMap.insert(sdb::Coord2(objI, i), 0);
    	}

   	}
	delete[] cs;

}

template<int P>
void IndexGridBuilder<P>::recordIndices(IndexGrid *grid)
{
	const int n = grid->numCells();
	for(int i=0;i<n;++i) {
		const Int2 &range = grid->c_cell()[i];
		if(range.x >= range.y) continue;

		for(int j=range.x;j<range.y;++j) {
			const int &objI = grid->c_indices()[j];
			m_objectCellMap.insert(sdb::Coord2(objI, i), 0);
		}
	}
}

template<int P>
template<typename Ti, typename Tr>
void IndexGridBuilder<P>::measureInstance(const Ti &instancer, int instanceI, Tr &rule)
{
	const int d = 1<<P;
    const int n = (d+1)*(d+1)*(d+1);
	int* ks = new int[n];
	rule.getLevelGrid(ks, 0, 0, P);
    
    float *distOnGrid = m_grid->value();
    float pos[3];
    for(int i=0;i<n;++i) {
        rule.computePosition(pos, ks[i]);
        float dist = instancer.mapDistance(pos, instanceI);
/// shorten the distance
        if(distOnGrid[i] > dist) distOnGrid[i] = dist;

    }
    
    delete[] ks;
    
/// store objects in cell 
	const int nc = d * d * d; 
	CellKH *cs = new CellKH[6 * nc];  
	rule. template getLevelCells<CellKH>(cs, 0, 0, P);  

	float b[6];
    for(int i=0;i<nc;++i) {

    	rule. template getCellBox<CellKH>(b, cs[i]);

    	rule.expandBox(b);

    	if(instancer.intersectBox(b, instanceI)) {
    		m_objectCellMap.insert(sdb::Coord2(instanceI, i), 0);
    	}

   	}
	delete[] cs;
}

}

}

#endif
