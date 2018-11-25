/*
 *  Entity.h
 *  aloe
 *
 *  Created by jian zhang on 4/30/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SDB_ENTITY_H
#define ALO_SDB_ENTITY_H

namespace alo {

namespace sdb {

class Entity
{
public:
	Entity(Entity * parent = 0);
	virtual ~Entity();
	
	Entity * parent() const;
	void setParent(Entity * parent);
	bool shareSameParent(Entity * another) const;
	
	virtual void display() const;
private:
	Entity *m_parent;
};

template<typename T>
class Single : public Entity
{
public:
	Single(Entity * parent = 0) : Entity(parent),
	m_p(0)
	{}
	
	T * data() {
		return m_p;
	}
	
	void setData(T * x) {
		m_p = x;
	}
	
private:
	T * m_p;
};

} // end of namespace sdb

}
#endif