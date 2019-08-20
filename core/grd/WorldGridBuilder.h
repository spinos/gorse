/*
 *  WorldGridBuilder.h
 *  gorse
 *
 *
 *  2019/5/5
 */

#ifndef ALO_GRD_WORLD_GRID_BUILDER_H
#define ALO_GRD_WORLD_GRID_BUILDER_H

#include "WorldGrid.h"
#include <boost/thread.hpp>

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
	template<typename Tr, typename Tcr>
	void mapCells(const float *b, const int &objI, Tr &rule, Tcr &cellRule);
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
	const int &n = instancer.numInstances();
	for(int i=0;i<n;++i) {
		instancer.getAabb(b, i);

		mapCells <Tr, Tcr>(b, i, gridRule, cellRule);
        
	}

	buildCells<Ti, Tcb, Tcr>(instancer, cellBuilder, cellRule);

}

template<typename T, typename Tc>
template<typename Tr, typename Tcr>
void WorldGridBuilder<T, Tc>::mapCells(const float *b, const int &objI, Tr &rule, Tcr &cellRule)
{
	rule.calcCellCoords(b);
	
    float cellBox[6];
	const int n = rule.numKeys();
	for(int i=0;i<n;++i) {
		const T & ki = rule.key(i);
		Tc *ci = m_grid->findCell(ki);
		if(!ci) {
			m_toRebuild = true;

			rule.calcCellAabb(cellBox, ki);

			Tc acell;
			acell.create(1<<cellRule.P());
			acell.setAabb(cellBox);

			ci = m_grid->addCell(ki, acell);
			std::cout << ".";
		}

		m_objectCellMap.insert(sdb::Coord2(objI, ki), 0);
	}

}

template<typename T, typename Tc>
template<typename Ti, typename Tcb, typename Tcr>
void WorldGridBuilder<T, Tc>::buildCells(const Ti &instancer, Tcb &cellBuilder, Tcr &cellRule)
{
    std::cout << "\n build world cells ";
	const int ninst = m_objectCellMap.size();
	int nmc = 0;
	int preInd = -1;

#define TEST_MT_SAMPLE

#ifdef TEST_MT_SAMPLE
#define NUM_SAMPLE_THREAD 8
    Ti::OutSampleTyp sampleData[NUM_SAMPLE_THREAD];
    boost::thread sampleThread[NUM_SAMPLE_THREAD];
    int objectInd[NUM_SAMPLE_THREAD];
    int threadCount = 0;
#else
    Ti::OutSampleTyp sampleData;
#endif
    
	sdb::L3Node<sdb::Coord2, int, 1024> *block = m_objectCellMap.begin();
	while(block) {
		for (int i=0;i<block->count();++i) { 
        	
        	const sdb::Coord2 &ci = block->key(i);

			if(preInd < ci.y) {

				if(preInd > -1) {
					nmc++;
#ifdef TEST_MT_SAMPLE
                    if(threadCount > 0) {
                        for(int t=0;t<threadCount;++t) {
                            sampleThread[t].join();
                        }
                        
                        for(int t=0;t<threadCount;++t) {
                            cellBuilder. template measure<Ti::OutSampleTyp, Tcr>(sampleData[t], objectInd[t], cellRule);
                        }
                        
                        threadCount = 0;
                    }
#endif
					cellBuilder.detach();
				}
                
				preInd = ci.y;

				Tc *cell = m_grid->findCell(ci.y);
				if(!cell) {
					std::cout << "\n ERROR WorldGridBuilder::buildCells cannot find cell " << ci.y;
					continue;
				}

				cellRule.setBBox((const float *)&cell->bbox());
				
				cellBuilder.attach(cell->_grid);
			}
#ifdef TEST_MT_SAMPLE            
            objectInd[threadCount] = ci.x;
            sampleData[threadCount].clear();
            sampleThread[threadCount] = boost::thread( 
            boost::bind(&Ti::genInstanceSamples, &instancer, &sampleData[threadCount], ci.x ) );
            
            threadCount++;
            
            if(threadCount == NUM_SAMPLE_THREAD) {
                for(int t=0;t<threadCount;++t) {
                    sampleThread[t].join();
                }
                
                for(int t=0;t<threadCount;++t) {
                    cellBuilder. template measure<Ti::OutSampleTyp, Tcr>(sampleData[t], objectInd[t], cellRule);
                }
                
                threadCount = 0;
            }
#else
			sampleData.clear();
			instancer.genInstanceSamples(&sampleData, ci.x);

			cellBuilder. template measure<Ti::OutSampleTyp, Tcr>(sampleData, ci.x, cellRule);
#endif
		}
        std::cout << ".";

		block = m_objectCellMap.next(block);
	}

	if(preInd > -1) {
		nmc++;
#ifdef TEST_MT_SAMPLE
        if(threadCount > 0) {
            for(int t=0;t<threadCount;++t) {
                sampleThread[t].join();
            }
            
            for(int t=0;t<threadCount;++t) {
                cellBuilder. template measure<Ti::OutSampleTyp, Tcr>(sampleData[t], objectInd[t], cellRule);
            }
            
            threadCount = 0;
        }
#endif           
		cellBuilder.detach();
	}

	m_objectCellMap.clear();

	std::cout << "\n INFO WorldGridBuilder built "<<ninst<<" instance in "<<nmc<<" cell ";

}

} /// end of namespace grd

} /// end of namespace alo

#endif
