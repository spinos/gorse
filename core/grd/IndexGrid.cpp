/*
 *  IndexGrid.cpp
 *  gorse
 *
 *  2019/4/28
 */

#include "IndexGrid.h"

namespace alo {
    
namespace grd {

IndexGrid::IndexGrid() :
m_numIndices(0)
{}

IndexGrid::~IndexGrid()
{}

void IndexGrid::create(const int& x)
{
	PGridTyp::create(x);
	const int nv = numValues();
	for(int i=0;i<nv;++i) value()[i] = 1e10f;

	const int nc = numCells();
	for(int i=0;i<nc;++i) cell()[i] = Int2(0,0);
}

void IndexGrid::setAabb(const float *b)
{
    memcpy(m_aabb, b, 24);
    float orih[4];
    
    orih[3] = b[3] - b[0];
    if(orih[3] < b[4] - b[1] ) orih[3] = b[4] - b[1];
    if(orih[3] < b[5] - b[2] ) orih[3] = b[5] - b[2];

    orih[0] = (b[0] + b[3]) * .5f - orih[3] * .5f;
    orih[1] = (b[1] + b[4]) * .5f - orih[3] * .5f;
    orih[2] = (b[2] + b[5]) * .5f - orih[3] * .5f;
    orih[3] *= 1.f / (float)resolution();
    
    PGridTyp::setOriginCellSize(orih);
}

int *IndexGrid::createIndices(int n)
{ 
    m_numIndices = n;
    m_indices.resetBuffer(n); 
    return m_indices.data();
}

int *IndexGrid::indices()
{ return m_indices.data(); }

const int *IndexGrid::c_indices() const
{ return m_indices.c_data(); }

const float *IndexGrid::aabb() const
{ return m_aabb; }

const int &IndexGrid::numIndices() const
{ return m_numIndices; }

bool IndexGrid::isCellEmpty(int i) const
{
    const Int2 &r = c_cell()[i];
    return r.x >= r.y;
}

void IndexGrid::verbose() const
{
    std::cout << " IndexGrid rez "<<resolution()
    <<"\n origin ("<<originCellSize()[0]<<", "<<originCellSize()[1]<<", "<<originCellSize()[2]
    <<") cell_size "<<originCellSize()[3]
    <<"\n aabb (("<<m_aabb[0]<<", "<<m_aabb[1]<<", "<<m_aabb[2]
    <<"), ("<<m_aabb[3]<<", "<<m_aabb[4]<<", "<<m_aabb[5]
    <<") )\n n index "<<numIndices();

    //const int nv = numValues();
    //for(int i=0;i<nv;++i) std::cout << " " << c_value()[i];

    //const int nc = numCells();
    //for(int i=0;i<nc;++i) std::cout << " " << c_cell()[i].x << ":" << c_cell()[i].y;
}

} /// end of namespace grd

} /// end of namespace alo
