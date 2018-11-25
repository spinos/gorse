/*
 *  WorldGrid.h
 *  
 *
 *  Created by jian zhang on 1/2/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 *  Highest level of spatial division, using grid(x,y,z) as key
 */

#pragma once
#include <sdb/Sequence.h>
#include <math/BoundingBox.h>

namespace alo {

namespace sdb {

template<typename ChildType, typename ValueType>
class WorldGrid : public Sequence<Coord3>
{
	BoundingBox m_bbox;
/// size of cell
	float m_gridSize;
	
public:
	WorldGrid(Entity * parent = NULL);
	virtual ~WorldGrid();
	
	void setGridSize(float x);
	
	ChildType * value();
	
	const Coord3 & key() const;
	
	ChildType * insertChild(const float * at);
	ChildType * insertCell(const Coord3 & x);
	
	void insertChildValue(const Coord3 & x , ChildType * v);
	
/// put v into grid
	void insert(const float * at, ValueType * v);
	void insert(const Coord3 & x , ValueType * v);
	
/// find grid coord of point(x,y,z)
	const Coord3 gridCoord(const float * p) const;

	void calculateBBox();
	const BoundingBox coordToGridBBox(const Coord3 & c) const;
	const BoundingBox & boundingBox() const;

	const float & gridSize() const;

/// relocate value into different cell	
	ValueType * displace(ValueType * v, const Vector3F & pcur, const Vector3F & pref);
	
	ChildType * findCell(const Vector3F & pref);
	ChildType * findCell(const Coord3 & c);
	
	Vector3F coordToCellCenter(const Coord3 & c) const;
	
	int elementCount();
	
	void storeCellNeighbors();
	void getCellNeighborBBox(BoundingBox & b,
					const Coord3 & cellCoord,
					const int & i) const;
	
	static int TwentySixNeighborCoord[26][3];
	
protected:
	float * gridSizeR();
    BoundingBox * boundingBoxR();
    const BoundingBox keyToGridBBox() const;
	
private:
	
};

template<typename ChildType, typename ValueType>
WorldGrid<ChildType, ValueType>::WorldGrid(Entity * parent) : Sequence<Coord3>(parent),
m_gridSize(32.f)
{}

template<typename ChildType, typename ValueType>
WorldGrid<ChildType, ValueType>::~WorldGrid()
{}

template<typename ChildType, typename ValueType>
ChildType * WorldGrid<ChildType, ValueType>::value() 
{ 
	ChildType * r = dynamic_cast<ChildType *>(Sequence<Coord3>::currentIndex() );
	if(r == NULL) throw "World Grid value null ";
	return r;
}

template<typename ChildType, typename ValueType>
const Coord3 & WorldGrid<ChildType, ValueType>::key() const 
{ return Sequence<Coord3>::currentKey(); }

template<typename ChildType, typename ValueType>
void WorldGrid<ChildType, ValueType>::setGridSize(float x)
{ m_gridSize = (x>32.f) ? x : 32.f; }

template<typename ChildType, typename ValueType>
const Coord3 WorldGrid<ChildType, ValueType>::gridCoord(const float * p) const
{
	Coord3 r;
	r.x = p[0] / m_gridSize; if(p[0] < 0.f) r.x--;
	r.y = p[1] / m_gridSize; if(p[1] < 0.f) r.y--;
	r.z = p[2] / m_gridSize; if(p[2] < 0.f) r.z--;
	return r;
}

template<typename ChildType, typename ValueType>
ChildType * WorldGrid<ChildType, ValueType>::insertChild(const float * at)
{
	const Coord3 x = gridCoord(at);
	return insertCell(x);
}

template<typename ChildType, typename ValueType>
ChildType * WorldGrid<ChildType, ValueType>::insertCell(const Coord3 & x)
{
	Pair<Coord3, Entity> * p = Sequence<Coord3>::insert(x);
	if(!p->index)
		p->index = new ChildType(this);

	return static_cast<ChildType *>(p->index);
}

template<typename ChildType, typename ValueType>
void WorldGrid<ChildType, ValueType>::insertChildValue(const Coord3 & x , ChildType * v)
{
	Pair<Coord3, Entity> * p = Sequence<Coord3>::insert(x);
	p->index = v;
}

template<typename ChildType, typename ValueType>
void WorldGrid<ChildType, ValueType>::insert(const float * at, ValueType * v) 
{
	const Coord3 x = gridCoord(at);
	insert(x, v);
}

template<typename ChildType, typename ValueType>
void WorldGrid<ChildType, ValueType>::insert(const Coord3 & x , ValueType * v)
{
	Pair<Coord3, Entity> * p = Sequence<Coord3>::insert(x);
	if(!p) {
	     throw "world grid cannot insert";
	}
	if(!p->index) p->index = new ChildType(this);
	ChildType *d = static_cast<ChildType *>(p->index);
	try {
	d->insert(v->key, v);
	} catch (const char * ex) {
		std::cerr<<" world grid insert caught "<<ex;
	} catch(...) {
		std::cerr<<" world grid insert caught something";
	}
}

template<typename ChildType, typename ValueType>
void WorldGrid<ChildType, ValueType>::calculateBBox()
{
	m_bbox.reset();
	begin();
	while(!end()) {
		m_bbox.expandBy(coordToGridBBox( key() ));
		next();
	}
}

template<typename ChildType, typename ValueType>
const BoundingBox WorldGrid<ChildType, ValueType>::coordToGridBBox(const Coord3 & c) const
{
	BoundingBox bb;
	bb.setMin(m_gridSize * c.x, m_gridSize * c.y, m_gridSize * c.z);
	bb.setMax(m_gridSize * (c.x + 1), m_gridSize * (c.y + 1), m_gridSize * (c.z + 1));
	return bb;
}

template<typename ChildType, typename ValueType>
Vector3F WorldGrid<ChildType, ValueType>::coordToCellCenter(const Coord3 & c) const
{
	return Vector3F(m_gridSize * (.5f + c.x), 
	                m_gridSize * (.5f + c.y), 
	                m_gridSize * (.5f + c.z) );
}

template<typename ChildType, typename ValueType>
const BoundingBox & WorldGrid<ChildType, ValueType>::boundingBox() const
{ return m_bbox; }

template<typename ChildType, typename ValueType>
const float & WorldGrid<ChildType, ValueType>::gridSize() const
{ return m_gridSize; }

template<typename ChildType, typename ValueType>
ValueType * WorldGrid<ChildType, ValueType>::displace(ValueType * v, const Vector3F & pcur, const Vector3F & pref)
{
	Coord3 c1 = gridCoord((float *)&pcur);
	Coord3 c0 = gridCoord((float *)&pref);
	if(c0 == c1) return NULL;
	
	ValueType * nv = new ValueType;
	nv->key = v->key;
	nv->index = v->index;
	
	Pair<Entity *, Entity> p = findEntity(c0);
	if(p.index) {
		ChildType * g = static_cast<ChildType *>(p.index);
		g->remove(v->key);
		if(g->isEmpty())
			remove(c0);
	}
	
	insert(c1, nv);
	
	return nv;
}

template<typename ChildType, typename ValueType>
ChildType * WorldGrid<ChildType, ValueType>::findCell(const Vector3F & pref)
{
	Coord3 c0 = gridCoord((float *)&pref);
	return findCell(c0);
}

template<typename ChildType, typename ValueType>
ChildType * WorldGrid<ChildType, ValueType>::findCell(const Coord3 & c)
{
	Pair<Entity *, Entity> p = findEntity(c);
	if(p.index) {
		ChildType * g = static_cast<ChildType *>(p.index);
		return g;
	}
	return NULL;
}

template<typename ChildType, typename ValueType>
float * WorldGrid<ChildType, ValueType>::gridSizeR()
{ return &m_gridSize; }

template<typename ChildType, typename ValueType>
BoundingBox * WorldGrid<ChildType, ValueType>::boundingBoxR()
{ return &m_bbox; }

template<typename ChildType, typename ValueType>
const BoundingBox WorldGrid<ChildType, ValueType>::keyToGridBBox() const
{ return coordToGridBBox(key() ); }

template<typename ChildType, typename ValueType>
int WorldGrid<ChildType, ValueType>::elementCount()
{
	int c = 0;
	begin();
	while(!end() ) {
	
		c += value()->size();
		next();
	}
	return c;
}

template<typename ChildType, typename ValueType>
int WorldGrid<ChildType, ValueType>::TwentySixNeighborCoord[26][3] = {
{-1, 0, 0}, // face
{ 1, 0, 0},
{ 0,-1, 0},
{ 0, 1, 0},
{ 0, 0,-1},
{ 0, 0, 1},
{-1,-1,-1}, // vertex
{ 1,-1,-1},
{-1, 1,-1},
{ 1, 1,-1},
{-1,-1, 1},
{ 1,-1, 1},
{-1, 1, 1},
{ 1, 1, 1},
{-1, 0,-1}, // edge
{ 1, 0,-1},
{-1, 0, 1},
{ 1, 0, 1},
{ 0,-1,-1},
{ 0, 1,-1},
{ 0,-1, 1},
{ 0, 1, 1},
{-1,-1, 0},
{ 1,-1, 0},
{-1, 1, 0},
{ 1, 1, 0}
};

template<typename ChildType, typename ValueType>
void WorldGrid<ChildType, ValueType>::storeCellNeighbors()
{
	begin();
	while(!end() ) {
	
		ChildType * c = value();
		
		const Coord3 & k = key();
		for(int i=0;i<26;++i) {
			const Coord3 ki = k + Coord3(TwentySixNeighborCoord[i][0],
									TwentySixNeighborCoord[i][1],
									TwentySixNeighborCoord[i][2]);
			ChildType * nei = findCell(ki);
			c->setCellNeighbor(nei, i);
			
		}
		
		next();
	}
}

template<typename ChildType, typename ValueType>
void WorldGrid<ChildType, ValueType>::getCellNeighborBBox(BoundingBox & b,
					const Coord3 & cellCoord,
					const int & i) const
{
	const Coord3 k = cellCoord + Coord3(TwentySixNeighborCoord[i][0],
									TwentySixNeighborCoord[i][1],
									TwentySixNeighborCoord[i][2]);
	b.setMin(m_gridSize * k.x, m_gridSize * k.y, m_gridSize * k.z);
	b.setMax(m_gridSize * (k.x + 1), m_gridSize * (k.y + 1), m_gridSize * (k.z + 1));
	
}

} //end namespace sdb

}
