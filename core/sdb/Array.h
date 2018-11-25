/*
 *  Array.h
 *  aloe
 *
 *  sequence of value
 *
 */

#ifndef ALO_SDB_ARRAY_H
#define ALO_SDB_ARRAY_H

#include "Sequence.h"
namespace alo {
namespace sdb {

template<typename T>
class Single : public Entity
{
	T * m_p;
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
	
};

template<typename KeyType, typename ValueType>
class Array : public Sequence<KeyType>
{
public:
	typedef Single<ValueType> SingleType;
	
    Array(Entity * parent = NULL) : Sequence<KeyType>(parent) {}
	
	virtual ~Array() {}
    
    void insert(const KeyType & x, ValueType * v) {
		Pair<KeyType, Entity> * p = Sequence<KeyType>::insert(x);
		if(p == NULL) {
		    throw "array cannot insert";
			return;
		}
		
		if(p->index == NULL) {
			try {
				p->index = new SingleType();
			} catch (std::bad_alloc& ba) {
				std::cerr << " array insert caught bad_alloc: "<< ba.what();
				return;
			} catch(...) {
				throw " array insert caught alloc index";
				return;
			}
		}
		
		try {
			SingleType * d = dynamic_cast<SingleType *>(p->index);
			d->setData(v);
		} catch(...) {
			throw "array insert caught set data";
		}
	}
	
	ValueType * value() const {
		try {
		SingleType * s = dynamic_cast<SingleType *>(Sequence<KeyType>::currentIndex());
		if(s==NULL) throw "array cast null single";
		
		ValueType * r = s->data();
		if(r==NULL) throw "array value null";
		return r;
		} catch(...) {
			throw "array wrong value";
		}
	}
	
	ValueType * find(const KeyType & k, MatchFunction::Condition mf = MatchFunction::mExact, KeyType * extraKey = NULL) const
	{			
		Pair<Entity *, Entity> g = Sequence<KeyType>::findEntity(k, mf, extraKey);

		if(!g.index) return NULL;
		
		SingleType * s = dynamic_cast<SingleType *>(g.index);
		if(s == NULL) throw "array find null single";
		
		return s->data();
	}
	
	virtual void clear() 
	{
		Sequence<KeyType>::begin();
		while(!Sequence<KeyType>::end()) {
			ValueType * p = value();
			if(p) {
				delete p;
			}
			Sequence<KeyType>::next();
		}
		
		Sequence<KeyType>::clear();
	}
	
/// keep data
	virtual void clearSequence() 
	{
		Sequence<KeyType>::clear();
	}

private:
	
};
} //end namespace sdb
}

#endif
