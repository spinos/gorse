/*
 *  IndexGrid.cpp
 *  gorse
 *
 *  2019/4/26
 */

#include "IndexGrid.h"

namespace alo {

IndexGrid::IndexGrid()
{}

IndexGrid::~IndexGrid()
{}

void IndexGrid::setResolution(const int& x)
{
	PGridTyp::setResolution(x);
	const int nv = numValues();
	for(int i=0;i<nv;++i) value()[i] = 1e10f;

	const int nc = numCells();
	for(int i=0;i<nc;++i) cell()[i] = Int2(0,0);
}

} /// end of namespace alo
