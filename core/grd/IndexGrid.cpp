/*
 *  IndexGrid.cpp
 *  gorse
 *
 *  2019/4/28
 */

#include "IndexGrid.h"

namespace alo {
    
namespace grd {

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

void IndexGrid::setAabb(const float *b)
{
    float orih[4];
    orih[0] = (b[0] + b[3]) * .5f;
    orih[1] = (b[1] + b[4]) * .5f;
    orih[2] = (b[2] + b[5]) * .5f;
    orih[3] = b[3] - b[0];
    if(orih[3] < b[4] - b[1] ) orih[3] = b[4] - b[1];
    if(orih[3] < b[5] - b[2] ) orih[3] = b[5] - b[2];
    orih[3] *= 1.f / (float)resolution();
    
    PGridTyp::setOriginCellSize(orih);
}

}

} /// end of namespace alo
