/*
 *  L2Tree.h
 *  sdb
 *
 *  sparse key indexing and searching in 2 levels
 *  root and leaf[]
 *  data stored in connected leaves
 *
 */

#ifndef SDB_L2_TREE_H
#define SDB_L2_TREE_H

#include "L3DataIterator.h"

namespace alo {

namespace sdb {

template <typename KeyType, typename DataType, int Dim0, int Dim1>
class L2Tree {

typedef L3Node<KeyType, DataType, Dim1> LeafNodeType;
typedef L3Node<KeyType, LeafNodeType *, Dim0> RootNodeType;

	RootNodeType m_root;

public:
	L2Tree();
	virtual ~L2Tree();

/// initial value at x
/// return data ptr null if failed
	DataType *insert(const KeyType &x, const DataType &a, bool writeExisting=true);
	bool remove(const KeyType &x);
	DataType *find(const KeyType &x);

	int size() const;
	void printDetail() const;

	LeafNodeType *begin();
	LeafNodeType *next(const LeafNodeType *x);
/// at leaf and location of x
/// if not found try last < x then first > x
	L3DataIterator<KeyType, DataType, Dim1> begin(const KeyType &x) const;

private:

};

template <typename KeyType, typename DataType, int Dim0, int Dim1>
L2Tree<KeyType, DataType, Dim0, Dim1>::L2Tree()
{}

template <typename KeyType, typename DataType, int Dim0, int Dim1>
L2Tree<KeyType, DataType, Dim0, Dim1>::~L2Tree()
{
	for(int i=0;i<m_root.count();++i)
		delete m_root.value(i);
}

template <typename KeyType, typename DataType, int Dim0, int Dim1>
DataType *L2Tree<KeyType, DataType, Dim0, Dim1>::insert(const KeyType &x, const DataType &a, bool writeExisting)
{
	DataType *rp = NULL;
	if(m_root.isEmpty()) {
		LeafNodeType *leaf = new LeafNodeType;
		int loc = leaf->insert(x, a);
		rp = &leaf->value(loc);
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
	loc = leaf->insert(x, a, writeExisting);
	rp = &leaf->value(loc);

	if(leaf->isFull()) {
		LeafNodeType *right = leaf->spawn();
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
bool L2Tree<KeyType, DataType, Dim0, Dim1>::remove(const KeyType &x)
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
		delete leaf;

	} else {
		m_root.key(loc) = leaf->key(0);
	}
	return true; 
}

template <typename KeyType, typename DataType, int Dim0, int Dim1>
DataType *L2Tree<KeyType, DataType, Dim0, Dim1>::find(const KeyType &x)
{
	if(m_root.isEmpty()) return NULL;
	KeyType b;
	int loc = m_root.keyLeft(x, b);
	if(loc < 0) return NULL;

	LeafNodeType *leaf = m_root.value(loc);
	return leaf->find(x);
}

template <typename KeyType, typename DataType, int Dim0, int Dim1>
int L2Tree<KeyType, DataType, Dim0, Dim1>::size() const
{
	if(m_root.isEmpty()) return 0;
	int s = 0;
	LeafNodeType *leaf = m_root.value(0);
	for(;;) {
		s += leaf->count();
		leaf = leaf->rightSibling();
		if(!leaf) break;
	}
	return s;
}

template <typename KeyType, typename DataType, int Dim0, int Dim1>
L3Node<KeyType, DataType, Dim1> *L2Tree<KeyType, DataType, Dim0, Dim1>::begin()
{
	if(m_root.isEmpty()) return NULL;
	return m_root.value(0);
}

template <typename KeyType, typename DataType, int Dim0, int Dim1>
L3Node<KeyType, DataType, Dim1> *L2Tree<KeyType, DataType, Dim0, Dim1>::next(const L3Node<KeyType, DataType, Dim1> *x)
{ return x->rightSibling(); }

template <typename KeyType, typename DataType, int Dim0, int Dim1>
L3DataIterator<KeyType, DataType, Dim1> L2Tree<KeyType, DataType, Dim0, Dim1>::begin(const KeyType &x) const
{
	L3DataIterator<KeyType, DataType, Dim1> it;
	if(m_root.isEmpty()) return it;
	KeyType b;
	int loc = m_root.keyLeft(x, b);
	if(loc < 0) loc = m_root.keyRight(x, b);
	if(loc < 0) return it;

	LeafNodeType *leaf = m_root.value(loc);
	loc = leaf->keyLeft(x, b);
	if(loc < 0) loc = leaf->keyRight(x, b);
	if(loc < 0) return it;

	it.first = b;
	it.second = &leaf->value(loc);
	it.m_node = leaf;
	it.m_loc = loc;
	return it;
}

template <typename KeyType, typename DataType, int Dim0, int Dim1>
void L2Tree<KeyType, DataType, Dim0, Dim1>::printDetail() const
{
	std::cout << "\n l2 tree root " << m_root.count();
	m_root.printDetail();

	if(m_root.isEmpty()) return;

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
