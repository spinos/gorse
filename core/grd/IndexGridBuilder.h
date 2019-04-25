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

namespace alo {

template<int P>
class IndexGridBuilder {

	IndexGrid *m_grid;

public:

	IndexGridBuilder();
	~IndexGridBuilder();

/// extract existing indices in grid
	void attach(IndexGrid *grid);
/// finish grid
	void dettach();
/// distance to sample
/// index in cell
/// T is sample type
/// Tr is build rule
	template<typename T, typename Tr>
	void measure(const T &sample, Tr &rule);

protected:

private:

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
}

template<int P>
void IndexGridBuilder<P>::dettach()
{
}

template<int P>
template<typename T, typename Tr>
void IndexGridBuilder<P>::measure(const T &sample, Tr &rule)
{
    const int d = 1<<P;
    const int n = (d+1)*(d+1)*(d+1);
	int* ks = new int[n];
	rule.getLevelGrid(ks, 0, 0, P);
    
    float *distOnGrid = m_grid->value();
    float pos[3];
    for(int i=0;i<n;++i) {
        rule.computePosition(pos, ks[i]);
        const float dist = sample.mapDistance(pos);
/// shorten the distance
        if(distOnGrid[i] > dist) distOnGrid[i] = dist;
/// store object in cell      
        if(dist < 0) {           
        }
    }
    
    delete[] ks;
}

}

#endif
