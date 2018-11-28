/*
 *  L3Tree.h
 *  sdb
 *
 *  2 levels
 *  root and leaf[]
 *  data stored in connected leaves
 *
 */

#ifndef SDB_L3_TREE_H
#define SDB_L3_TREE_H

#include <iostream>
#include <vector>
#include "L3Node.h"

namespace alo {

namespace sdb {

template <typename KeyType, typename DataType, int Dim0, int Dim1>
class L3Tree {

typedef L3Node<KeyType, DataType, Dim1> LeafNodeType;
typedef L3Node<KeyType, LeafNodeType *, Dim0> RootNodeType;
typedef std::vector<LeafNodeType *> LeafList;

	RootNodeType m_root;
	LeafList m_leafNodes;

public:
	L3Tree();
	virtual ~L3Tree();

/// initial value at x
/// return data ptr null if failed
	DataType *insert(const KeyType &x, const DataType &a);
	bool remove(const KeyType &x);
	DataType *find(const KeyType &x);

	int size() const;
	void printDetail() const;

	void add(const DataType &x);

	typename LeafList::iterator begin();
	typename LeafList::iterator end();

private:
	void removeLeaf(LeafNodeType *x);

};

template <typename KeyType, typename DataType, int Dim0, int Dim1>
L3Tree<KeyType, DataType, Dim0, Dim1>::L3Tree()
{}

template <typename KeyType, typename DataType, int Dim0, int Dim1>
L3Tree<KeyType, DataType, Dim0, Dim1>::~L3Tree()
{
	std::vector<LeafNodeType *>::iterator it = m_leafNodes.begin();
	for(;it!=m_leafNodes.end();++it)
		delete *it;
}

template <typename KeyType, typename DataType, int Dim0, int Dim1>
DataType *L3Tree<KeyType, DataType, Dim0, Dim1>::insert(const KeyType &x, const DataType &a)
{
	DataType *rp = NULL;
	if(m_root.isEmpty()) {
		LeafNodeType *leaf = new LeafNodeType;
		int loc = leaf->insert(x, a);
		rp = &leaf->value(loc);
		m_leafNodes.push_back(leaf);
		m_root.insert(x, leaf);
		return rp;
	}

	KeyType b;
	int loc = m_root.keyLeft(x, b);
	if(loc < 0) {
		loc = 0;
		m_root.key(0) = x;
	}

	LeafNodeType *leaf = m_root.value(loc);
	loc = leaf->insert(x, a);
	rp = &leaf->value(loc);

	if(leaf->isFull()) {
		LeafNodeType *right = leaf->spawn();
		m_leafNodes.push_back(right);
		m_root.insert(right->key(0), right);

		LeafNodeType *cousin = leaf->rightSibling();
		leaf->connectToRightSibling(right);
		right->connectToLeftSibling(leaf);
		right->connectToRightSibling(cousin);
		if(cousin) cousin->connectToLeftSibling(right);

		rp = find(x);
	}

	return rp;
}

template <typename KeyType, typename DataType, int Dim0, int Dim1>
bool L3Tree<KeyType, DataType, Dim0, Dim1>::remove(const KeyType &x)
{ 
	KeyType b;
	int loc = m_root.keyLeft(x, b);
	if(loc < 0) return false;

	LeafNodeType *leaf = m_root.value(loc);
	leaf->remove(x);

	if(leaf->isEmpty()) {

		LeafNodeType *left = leaf->leftSibling();
		LeafNodeType *right = leaf->rightSibling();
		if(left) left->connectToRightSibling(right);
		if(right) right->connectToLeftSibling(left);

		m_root.remove(x);
		removeLeaf(leaf);
		delete leaf;

	} else {
		m_root.key(loc) = leaf->key(0);
	}
	return true; 
}

template <typename KeyType, typename DataType, int Dim0, int Dim1>
DataType *L3Tree<KeyType, DataType, Dim0, Dim1>::find(const KeyType &x)
{
	if(m_root.isEmpty()) return NULL;
	KeyType b;
	int loc = m_root.keyLeft(x, b);
	if(loc < 0) return NULL;

	LeafNodeType *leaf = m_root.value(loc);
	return leaf->find(x);
}

template <typename KeyType, typename DataType, int Dim0, int Dim1>
void L3Tree<KeyType, DataType, Dim0, Dim1>::removeLeaf(LeafNodeType *x)
{
	std::vector<LeafNodeType *>::iterator it = m_leafNodes.begin();
	for(;it!=m_leafNodes.end();++it) {
		if(*it == x) {
			m_leafNodes.erase(it);
			return;
		}
	}
}

template <typename KeyType, typename DataType, int Dim0, int Dim1>
void L3Tree<KeyType, DataType, Dim0, Dim1>::add(const DataType &x)
{
	std::vector<LeafNodeType *>::iterator it = m_leafNodes.begin();
	for(;it!=m_leafNodes.end();++it)
		(*it)->add(x);
}

template <typename KeyType, typename DataType, int Dim0, int Dim1>
int L3Tree<KeyType, DataType, Dim0, Dim1>::size() const
{
	int s = 0;
	std::vector<LeafNodeType *>::const_iterator it = m_leafNodes.begin();
	for(;it!=m_leafNodes.end();++it) {
		s += (*it)->count();
	}
	return s;
}

template <typename KeyType, typename DataType, int Dim0, int Dim1>
typename std::vector<L3Node<KeyType, DataType, Dim1> *>::iterator L3Tree<KeyType, DataType, Dim0, Dim1>::begin()
{ return m_leafNodes.begin(); }

template <typename KeyType, typename DataType, int Dim0, int Dim1>
typename std::vector<L3Node<KeyType, DataType, Dim1> *>::iterator L3Tree<KeyType, DataType, Dim0, Dim1>::end()
{ return m_leafNodes.end(); }

template <typename KeyType, typename DataType, int Dim0, int Dim1>
void L3Tree<KeyType, DataType, Dim0, Dim1>::printDetail() const
{
	std::cout << "\n l3 tree root " << m_root.count();
	m_root.printDetail();

	if(m_leafNodes.size() < 1) return;

	LeafNodeType *leaf = m_root.value(0);
	for(int i=0;;++i) {
		std::cout << "\n leaf " << i;
		leaf->printKeyRange();
		leaf = leaf->rightSibling();
		if(!leaf) break;
	}

	std::cout << "\n n data " << size();
	
}

}

}

#endif
