/*
 *  Entity.h
 *  aloe
 *
 *  parent
 *  |
 *  entity
 *
 */

#ifndef ALO_SDB_ENTITY_H
#define ALO_SDB_ENTITY_H

namespace alo {

namespace sdb {

class Entity
{
    Entity *m_parent;
    
public:
	Entity(Entity * parent = 0);
	virtual ~Entity();
	
	Entity * parent() const;
	void setParent(Entity * parent);
	bool shareSameParent(Entity * another) const;
	
	virtual void display() const;
private:
	
};

} // end of namespace sdb

}
#endif