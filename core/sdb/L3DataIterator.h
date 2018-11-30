#ifndef SDB_L3_DATA_ITERATOR_H
#define SDB_L3_DATA_ITERATOR_H

#include "L3Node.h"

namespace alo {

namespace sdb {

template <typename KeyType, typename DataType, int Dim1>
class L3DataIterator {

public:
	L3DataIterator();

	bool done() const;
	void next();
	
	KeyType first;
	DataType *second;
	L3Node<KeyType, DataType, Dim1> *m_node;
	int m_loc;

};

template <typename KeyType, typename DataType, int Dim1>
L3DataIterator<KeyType, DataType, Dim1>::L3DataIterator() :
m_node(NULL)
{}

template <typename KeyType, typename DataType, int Dim1>
bool L3DataIterator<KeyType, DataType, Dim1>::done() const
{ return !m_node; }

template <typename KeyType, typename DataType, int Dim1>
void L3DataIterator<KeyType, DataType, Dim1>::next()
{ 
	m_loc++;
	if(m_loc == m_node->count()) {
		m_node = m_node->rightSibling();
		if(!m_node) return;
		m_loc = 0;
	}
	first = m_node->key(m_loc);
	second = &m_node->value(m_loc);
}

}

}

#endif
