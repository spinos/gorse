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

template<typename T>
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
/// index in cell
/// Ts is sample array type, value must have _key and _pos
/// Tr is build rule
	template<typename Ts, typename Tr>
	void measure(const Ts &samples, int objI, Tr &rule);

protected:

private:
/// extract existing indices in grid
	void recordIndices(T *grid);

	struct CellKH {
		int _key;
		int _span;
	};
};

template<typename T>
LocalGridBuilder<T>::LocalGridBuilder()
{}

template<typename T>
LocalGridBuilder<T>::~LocalGridBuilder()
{}

template<typename T>
void LocalGridBuilder<T>::attach(T *grid)
{
	m_grid = grid;
}

template<typename T>
void LocalGridBuilder<T>::detach()
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

	m_grid->reset();

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
	int valueInd;
	
	block = m_objectCellMap.begin();
	while(block) {
		for (int i=0;i<block->count();++i) { 
			const sdb::Coord2 &ci = block->key(i);

			if(ci.y >= m_grid->numCells() ) {
				std::cout << "\n ERROR oor cell ind " << ci.y;
			}

			inds[offset] = ci.x;

			if(preCell != ci.y) {
				preCell = ci.y;
				countNonEmptyCells++;

				Int2 &cellBegin = m_grid->addCell(Int2(0,0), ci.y);

				cellBegin.x = offset;

				valueInd = m_grid->hasCell(ci.y);
			}

			offset++;

			Int2 &cellEnd = m_grid->mappedCell(valueInd);
			cellEnd.y = offset;

		}
		block = m_objectCellMap.next(block);
	}

	m_objectCellMap.clear();
    
}

template<typename T>
template<typename Ts, typename Tr>
void LocalGridBuilder<T>::measure(const Ts &samples, int objI, Tr &rule)
{
	int n = samples.size();
	const BoundingBox &b = rule.bbox();
	Ts::ValueTyp ap;
	sds::SpaceFillingVector<Ts::ValueTyp> destSamples;
	for(int i=0;i<n;++i) {
		const Ts::ValueTyp &si = samples[i];
		const float *q = (const float *)&si._pos;
		if(isPointOutsideBox(q, b.data())) continue;

		ap._key = rule.computeKeyAtLevel(q, rule.P());

		destSamples << ap;
	}

	n = destSamples.size();
	if(n < 1) return;

	destSamples.sort();

	int prek =-1;
	for(int i=0;i<n;++i) {
		const Ts::ValueTyp &si = destSamples[i];
		if(si._key != prek) {

			prek = si._key;

			const int celli = rule.computeCellInd(si._key, rule.P());

			if(celli >= m_grid->numCells() ) {
				std::cout << "\n ERROR oor cell ind " << celli;
				rule.printCoord(si._key);
			}

			m_objectCellMap.insert(sdb::Coord2(objI, celli), 0);
		}
	}
	
}

template<typename T>
void LocalGridBuilder<T>::recordIndices(T *grid)
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

}

}

#endif
