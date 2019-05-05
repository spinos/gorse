/*
 *  LocalGridBuilder.h
 *  gorse
 *
 *  P is level of grid
 *  2^(P*3) cells
 *
 *  2019/5/5
 */

#ifndef ALO_LOCAL_GRID_BUILDER_H
#define ALO_LOCAL_GRID_BUILDER_H

#include "LocalGrid.h"
#include <sdb/Types.h>
#include <sdb/L3Tree.h>
#include <sds/SpaceFillingVector.h>

namespace alo {
    
namespace grd {

template<typename T, int P>
class LocalGridBuilder {

	T *m_grid;
/// (object, cell) as key
	sdb::L3Tree<sdb::Coord2, int, 2048, 512, 1024 > m_objectCellMap;

public:

	LocalGridBuilder();
	~LocalGridBuilder();

	void attach(T *grid);
/// finish grid
	void detach();
/// distance to sample
/// index in cell
/// Ts is sample type
/// Tr is build rule
	template<typename Ts, typename Tr>
	void measure(const Ts &sample, int objI, Tr &rule);

	template<typename Ti, typename Tr>
	void measureInstance(const Ti &instancer, int instanceI, Tr &rule);

protected:

private:
/// extract existing indices in grid
	void recordIndices(T *grid);

	struct CellKH {
		int _key;
		int _span;
	};
};

template<typename T, int P>
LocalGridBuilder<T, P>::LocalGridBuilder()
{}

template<typename T, int P>
LocalGridBuilder<T, P>::~LocalGridBuilder()
{}

template<typename T, int P>
void LocalGridBuilder<T, P>::attach(T *grid)
{
	m_grid = grid;
	if(grid->numIndices()) recordIndices(grid);
}

template<typename T, int P>
void LocalGridBuilder<T, P>::detach()
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

	m_grid->buildBvh();

}

template<typename T, int P>
template<typename Ts, typename Tr>
void LocalGridBuilder<T, P>::measure(const Ts &sample, int objI, Tr &rule)
{
	const sds::SpaceFillingVector<Ts::SampleTyp> &orignalSamples = sample.samples();
	int n = orignalSamples.size();
	const BoundingBox &b = rule.bbox();
	Ts::SampleTyp ap;
	sds::SpaceFillingVector<Ts::SampleTyp> destSamples;
	for(int i=0;i<n;++i) {
		const Ts::SampleTyp &si = orignalSamples[i];
		const float *q = (const float *)&si._pos;
		if(isPointOutsideBox(q, b.data())) continue;

		ap._key = rule.computeKeyAtLevel(q, P);

		destSamples.push_back(ap);
	}

	destSamples.sort();
	n = destSamples.size();

	int prek =-1;
	for(int i=0;i<n;++i) {
		const Ts::SampleTyp &si = destSamples[i];
		if(si._key != prek) {

			prek = si._key;

			const int celli = rule.computeCellInd(si._key, P);
			m_objectCellMap.insert(sdb::Coord2(objI, celli), 0);
		}
	}

}

template<typename T, int P>
void LocalGridBuilder<T, P>::recordIndices(T *grid)
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

template<typename T, int P>
template<typename Ti, typename Tr>
void LocalGridBuilder<T, P>::measureInstance(const Ti &instancer, int instanceI, Tr &rule)
{
	const int d = 1<<P;
    
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
