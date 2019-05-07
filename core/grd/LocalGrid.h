/*
 *  LocalGrid.h
 *  gorse
 *
 *  cubic grid stores index range to object inside as cell values
 *  first n_obj indices are object ids
 *  ray-intersect nonempty cells using bvh
 *
 *  2019/5/4
 */

#ifndef ALO_GRD_LOCAL_GRID_H
#define ALO_GRD_LOCAL_GRID_H

#include <sds/CubicGrid.h>
#include <math/Int2.h>
#include <bvh/BVH.h>
#include <bvh/BVHBuilder.h>
#include <bvh/BVHSplit.h>
#include <bvh/BVHPrimitive.h>

namespace alo {
    
namespace grd {

template<typename T>
class LocalGrid : public sds::CubicGrid<T, Int2> {

typedef sds::CubicGrid<T, Int2> PGridTyp;

	SimpleBuffer<int> m_indices;
	int m_numIndices;
    int m_numObjects;
    BVH *m_bvh;

public:

	LocalGrid();
	virtual ~LocalGrid();
/// create grid by resolution x, initial distance to very large
/// indices range to [0,0]
	void create(const int& x);
    
    void setAabb(const float *b);
/// unique object id count
    void setNumObjects(int x);

    int *createIndices(int n);

    void buildBvh();

    const int *c_indices() const;
    
    const BoundingBox &bbox() const;

    const int &numIndices() const;
    const int &numObjects() const;
/// i-th cell range y > x
    bool isCellEmpty(int i) const;

    const BVH *boundingVolumeHierarchy() const;
    const BoundingBox &primitiveBox(int i) const;
    const int &primitiveIndex(int i) const;

    bool isEmpty() const;

    void verbose() const;
    
protected:
	int *indices();

private:

};

template<typename T>
LocalGrid<T>::LocalGrid() : m_bvh(nullptr),
m_numIndices(0),
m_numObjects(0)
{}

template<typename T>
LocalGrid<T>::~LocalGrid()
{
    if(m_bvh) delete m_bvh;
}

template<typename T>
void LocalGrid<T>::create(const int& x)
{
    PGridTyp::create(x);

    const int nc = numCells();
    for(int i=0;i<nc;++i) cell()[i] = Int2(0,0);
}

template<typename T>
void LocalGrid<T>::setAabb(const float *b)
{
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

template<typename T>
void LocalGrid<T>::setNumObjects(int x)
{ m_numObjects = x; }

template<typename T>
int *LocalGrid<T>::createIndices(int n)
{ 
    m_numIndices = n;
    m_indices.resetBuffer(n); 
    return m_indices.data();
}

template<typename T>
void LocalGrid<T>::buildBvh()
{
    if(m_bvh) delete m_bvh;
    m_bvh = new BVH;
    m_bvh->clear();

    BVHPrimitive ap;
    BoundingBox ab;

    const int nc = numCells();
    for(int i=0;i<nc;++i) {
        if(isCellEmpty(i)) continue;

        getCellBox(ab, i);

        ap.setBBox(ab);
/// cell ind as primitive ind
        ap.setIndex(i);


        m_bvh->addPrimitive(ap);
    }

    m_bvh->setRootLeaf();

    BVHSplit::InnerNumPrimitives = 16;
    BVHSplit::LeafNumPrimitives = 4;

    BVHBuilder::Build(m_bvh);
    
    //std::cout << *m_bvh;

}

template<typename T>
int *LocalGrid<T>::indices()
{ return m_indices.data(); }

template<typename T>
const int *LocalGrid<T>::c_indices() const
{ return m_indices.c_data(); }

template<typename T>
const BoundingBox &LocalGrid<T>::bbox() const
{ return m_bvh->aabb(); }

template<typename T>
const int &LocalGrid<T>::numIndices() const
{ return m_numIndices; }

template<typename T>
const int &LocalGrid<T>::numObjects() const
{ return m_numObjects; }

template<typename T>
bool LocalGrid<T>::isCellEmpty(int i) const
{
    const Int2 &r = c_cell()[i];
    return r.x >= r.y;
}

template<typename T>
const BVH *LocalGrid<T>::boundingVolumeHierarchy() const
{ return m_bvh; }

template<typename T>
const BoundingBox &LocalGrid<T>::primitiveBox(int i) const
{ return m_bvh->primitiveBox(i);}

template<typename T>
const int &LocalGrid<T>::primitiveIndex(int i) const
{ return m_bvh->primitiveIndex(i); }

template<typename T>
bool LocalGrid<T>::isEmpty() const
{ return m_numIndices < 1; }

template<typename T>
void LocalGrid<T>::verbose() const
{
    int nne = 0;
    const int nc = numCells();
    for(int i=0;i<nc;++i) {
        if( c_cell()[i].x < c_cell()[i].y) nne++;
    }
    std::cout << "\n LocalGrid rez "<<resolution()
    <<" origin ("<<originCellSize()[0]<<", "<<originCellSize()[1]<<", "<<originCellSize()[2]
    <<") cell_size "<<originCellSize()[3]
    <<"\n "<< numObjects() << " object " <<numIndices() - numObjects() << " instance in "<<nne<<" cell ";

}

}

}

#endif