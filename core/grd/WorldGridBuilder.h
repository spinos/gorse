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
#include <boost/chrono/include.hpp>

namespace alo {

namespace grd {

template<typename T, typename Tc>
class WorldGridBuilder {

/// key (object, cell) value (object_begin, object_end) 
/// storage range of bundled objects 
/// singular object object_begin = object_end
	sdb::L3Tree<sdb::Coord2, Int2, 2048, 512, 1024 > m_objectCellMap;
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
	void mapCells(const float *b, const Int2 &objI, Tr &rule, Tcr &cellRule);
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
    const std::vector<Int2> &smallBundles = instancer.clusters();
    Int2 currentBundle(-1, -1);
    std::vector<Int2>::const_iterator bundleIt = smallBundles.begin();
    if(bundleIt != smallBundles.end())
        currentBundle = *bundleIt;

    bool isBundled = false;
	float b[6];
	const int &n = instancer.numInstances();
	for(int i=0;i<n;++i) {
        
        if(i == currentBundle.x) {
            isBundled = true;
        } else if(i+1 == currentBundle.y) {
            isBundled = false;
            
            instancer.getClusterAabb(b, currentBundle.x, currentBundle.y);
            mapCells <Tr, Tcr>(b, currentBundle, gridRule, cellRule);
            
            bundleIt++;
            if(bundleIt != smallBundles.end()) {
                currentBundle = *bundleIt;
            } else
                currentBundle.set(-1,-1);
            
        } 

        if(!isBundled) {
            instancer.getAabb(b, i);
            mapCells <Tr, Tcr>(b, Int2(i,i), gridRule, cellRule);
        }
	}

	buildCells<Ti, Tcb, Tcr>(instancer, cellBuilder, cellRule);

}

template<typename T, typename Tc>
template<typename Tr, typename Tcr>
void WorldGridBuilder<T, Tc>::mapCells(const float *b, const Int2 &objI, Tr &rule, Tcr &cellRule)
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

		m_objectCellMap.insert(sdb::Coord2(objI.x, ki), objI);
	}

}

template<typename T, typename Tc>
template<typename Ti, typename Tcb, typename Tcr>
void WorldGridBuilder<T, Tc>::buildCells(const Ti &instancer, Tcb &cellBuilder, Tcr &cellRule)
{
    boost::chrono::system_clock::time_point t0 = boost::chrono::system_clock::now();
    
    std::cout << "\n build world cells ";
	const int ninst = m_objectCellMap.size();
	int nmc = 0;
	int preInd = -1;

#define TEST_MT_SAMPLE

#ifdef TEST_MT_SAMPLE
#define NUM_SAMPLE_THREAD 8
    Ti::OutSampleTyp sampleData[NUM_SAMPLE_THREAD];
    boost::thread sampleThread[NUM_SAMPLE_THREAD];
    int threadCount = 0;
#else
    Ti::OutSampleTyp sampleData;
#endif
    
	sdb::L3Node<sdb::Coord2, Int2, 1024> *block = m_objectCellMap.begin();
	while(block) {
		for (int i=0;i<block->count();++i) { 
        	
        	const sdb::Coord2 &ci = block->key(i);
            const Int2 &objRange = block->value(i);

			if(preInd < ci.y) {

				if(preInd > -1) {
					nmc++;
#ifdef TEST_MT_SAMPLE
                    if(threadCount > 0) {
                        for(int t=0;t<threadCount;++t) {
                            sampleThread[t].join();
                        }
                        
                        for(int t=0;t<threadCount;++t) {
                            cellBuilder. template measure<Ti::OutSampleTyp, Tcr>(sampleData[t], cellRule);
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
            sampleData[threadCount].clear();
            sampleThread[threadCount] = boost::thread( 
            boost::bind(&Ti::genInstanceSamples, &instancer, &sampleData[threadCount], objRange ) );
            
            threadCount++;
            
            if(threadCount == NUM_SAMPLE_THREAD) {
                for(int t=0;t<threadCount;++t) {
                    sampleThread[t].join();
                }
                
                for(int t=0;t<threadCount;++t) {
                    cellBuilder. template measure<Ti::OutSampleTyp, Tcr>(sampleData[t], cellRule);
                }
                
                threadCount = 0;
            }
#else
			sampleData.clear();
			instancer.genInstanceSamples(&sampleData, ci.x);

			cellBuilder. template measure<Ti::OutSampleTyp, Tcr>(sampleData, cellRule);
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
                cellBuilder. template measure<Ti::OutSampleTyp, Tcr>(sampleData[t], cellRule);
            }
            
            threadCount = 0;
        }
#endif           
		cellBuilder.detach();
	}

	m_objectCellMap.clear();
    
    boost::chrono::system_clock::time_point t1 = boost::chrono::system_clock::now();
    
    boost::chrono::duration<double> sec = t1 - t0;

	std::cout << "\n "<<ninst<<" instance in "<<nmc
                <<" cell \n finished in " << sec.count() << " seconds ";

}

} /// end of namespace grd

} /// end of namespace alo

#endif
