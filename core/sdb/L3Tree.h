/*
 *  L3Tree.h
 *  sdb
 *
 *  sparse key indexing and searching in 3 levels
 *  root, inner[], leaf[]
 *  data stored in connected leaves
 *
 */

#ifndef SDB_L3_TREE_H
#define SDB_L3_TREE_H

#include "L3DataIterator.h"

namespace alo {

namespace sdb {

template <typename KeyType, typename DataType, int Dim0, int Dim1, int Dim2>
class L3Tree {

typedef L3Node<KeyType, DataType, Dim2> LeafNodeType;
typedef L3Node<KeyType, LeafNodeType *, Dim1> InnerNodeType;
typedef L3Node<KeyType, InnerNodeType *, Dim0> RootNodeType;

	RootNodeType m_root;

public:
	L3Tree();
	virtual ~L3Tree();

/// initial value at x
/// return data ptr null if failed
	DataType *insert(const KeyType &x, const DataType &a, bool writeExisting=true);
	bool remove(const KeyType &x);
	DataType *find(const KeyType &x);
	void clear();

	int size() const;
	void printDetail() const;

	LeafNodeType *begin() const;
	LeafNodeType *next(const LeafNodeType *x) const;
/// at leaf and location of x
/// if not found try last < x then first > x
	L3DataIterator<KeyType, DataType, Dim2> begin(const KeyType &x) const;

private:

};

template <typename KeyType, typename DataType, int Dim0, int Dim1, int Dim2>
L3Tree<KeyType, DataType, Dim0, Dim1, Dim2>::L3Tree()
{}

template <typename KeyType, typename DataType, int Dim0, int Dim1, int Dim2>
L3Tree<KeyType, DataType, Dim0, Dim1, Dim2>::~L3Tree()
{ clear(); }

template <typename KeyType, typename DataType, int Dim0, int Dim1, int Dim2>
void L3Tree<KeyType, DataType, Dim0, Dim1, Dim2>::clear()
{
    for(int i=0;i<m_root.count();++i) {
		InnerNodeType *inner = m_root.value(i);
		for(int j=0;j<inner->count();++j)
			delete inner->value(j);
		delete inner;
	}
	m_root.setEmpty();
}

template <typename KeyType, typename DataType, int Dim0, int Dim1, int Dim2>
DataType *L3Tree<KeyType, DataType, Dim0, Dim1, Dim2>::insert(const KeyType &x, const DataType &a, bool writeExisting)
{
	DataType *rp = NULL;
	if(m_root.isEmpty()) {
		InnerNodeType *inner = new InnerNodeType;
		
		LeafNodeType *leaf = new LeafNodeType;
		int loc = leaf->insert(x, a);
		rp = &leaf->value(loc);

		inner->insert(x, leaf);
		m_root.insert(x, inner);
		return rp;
	}

	InnerNodeType *inner = m_root.value(0);

	KeyType b;
	int loc1 = m_root.keyLeft(x, b);
	if(loc1 < 0) {
		loc1 = 0;
		inner->key(0) = x;
		m_root.key(0) = x;
	}

	inner = m_root.value(loc1);
	int loc2 = inner->keyLeft(x, b);

	LeafNodeType *leaf = inner->value(loc2);
	int loc3 = leaf->insert(x, a, writeExisting);
	rp = &leaf->value(loc3);

	if(loc3 == 0) inner->key(loc2) = leaf->key(0);
	if(loc2 == 0) m_root.key(loc1) = inner->key(0);

	bool topoChanged = false;

	if(leaf->isFull()) {
		LeafNodeType *right = leaf->spawn();
		inner->insert(right->key(0), right);

		LeafNodeType *cousin = leaf->rightSibling();
		leaf->connectToRightSibling(right);
		right->connectToLeftSibling(leaf);
		right->connectToRightSibling(cousin);
		if(cousin) cousin->connectToLeftSibling(right);

		topoChanged = true;
	}

	if(inner->isFull()) {
		InnerNodeType *right = inner->spawn();
		m_root.insert(right->key(0), right);

		InnerNodeType *cousin = inner->rightSibling();
		inner->connectToRightSibling(right);
		right->connectToLeftSibling(inner);
		right->connectToRightSibling(cousin);
		if(cousin) cousin->connectToLeftSibling(right);

		topoChanged = true;
	}

	if(topoChanged)
		rp = find(x);

	return rp;
}

template <typename KeyType, typename DataType, int Dim0, int Dim1, int Dim2>
bool L3Tree<KeyType, DataType, Dim0, Dim1, Dim2>::remove(const KeyType &x)
{ 
	KeyType b;
	int loc1 = m_root.keyLeft(x, b);
	if(loc1 < 0) return false;

	InnerNodeType *inner = m_root.value(loc1);
	int loc2 = inner->keyLeft(x, b);

	LeafNodeType *leaf = inner->value(loc2);
	leaf->remove(x);

	if(leaf->isEmpty()) {

		LeafNodeType *left = leaf->leftSibling();
		LeafNodeType *right = leaf->rightSibling();
		if(left) left->connectToRightSibling(right);
		if(right) right->connectToLeftSibling(left);

		inner->remove(x);
		delete leaf;

	} else {
		inner->key(loc2) = leaf->key(0);
	}

	if(inner->isEmpty()) {

		InnerNodeType *left = inner->leftSibling();
		InnerNodeType *right = inner->rightSibling();
		if(left) left->connectToRightSibling(right);
		if(right) right->connectToLeftSibling(left);

		m_root.remove(x);
		delete inner;

	} else {
		m_root.key(loc1) = inner->key(0);
	}

	return true; 
}

template <typename KeyType, typename DataType, int Dim0, int Dim1, int Dim2>
DataType *L3Tree<KeyType, DataType, Dim0, Dim1, Dim2>::find(const KeyType &x)
{
	if(m_root.isEmpty()) return NULL;
	KeyType b;
	int loc1 = m_root.keyLeft(x, b);
	if(loc1 < 0) return NULL;

	InnerNodeType *inner = m_root.value(loc1);

	int loc2 = inner->keyLeft(x, b);
	LeafNodeType *leaf = inner->value(loc2);
	return leaf->find(x);
}

template <typename KeyType, typename DataType, int Dim0, int Dim1, int Dim2>
int L3Tree<KeyType, DataType, Dim0, Dim1, Dim2>::size() const
{
	if(m_root.isEmpty()) return 0;
	InnerNodeType *inner = m_root.value(0);
	LeafNodeType *leaf = inner->value(0);
	int s = 0;
	for(;;) {
		s += leaf->count();
		leaf = leaf->rightSibling();
		if(!leaf) break;
	}
	return s;
}

template <typename KeyType, typename DataType, int Dim0, int Dim1, int Dim2>
L3Node<KeyType, DataType, Dim2> *L3Tree<KeyType, DataType, Dim0, Dim1, Dim2>::begin() const
{
	if(m_root.isEmpty()) return NULL;
	InnerNodeType *inner = m_root.value(0);
	return inner->value(0);
}

template <typename KeyType, typename DataType, int Dim0, int Dim1, int Dim2>
L3Node<KeyType, DataType, Dim2> *L3Tree<KeyType, DataType, Dim0, Dim1, Dim2>::next(const L3Node<KeyType, DataType, Dim2> *x) const
{ return x->rightSibling(); }

template <typename KeyType, typename DataType, int Dim0, int Dim1, int Dim2>
L3DataIterator<KeyType, DataType, Dim2> L3Tree<KeyType, DataType, Dim0, Dim1, Dim2>::begin(const KeyType &x) const
{
	L3DataIterator<KeyType, DataType, Dim2> it;
	if(m_root.isEmpty()) return it;
	KeyType b;
	int loc1 = m_root.keyLeft(x, b);
	if(loc1 < 0) loc1 = m_root.keyRight(x, b);
	if(loc1 < 0) return it;

	InnerNodeType *inner = m_root.value(loc1);
	int loc2 = inner->keyLeft(x, b);
	if(loc2 < 0) loc2 = inner->keyRight(x, b);
	if(loc2 < 0) return it;

	LeafNodeType *leaf = inner->value(loc2);
	int loc3 = leaf->keyLeft(x, b);
	if(loc3 < 0) loc3 = leaf->keyRight(x, b);
	if(loc3 < 0) return it;

	it.first = b;
	it.second = &leaf->value(loc3);
	it.m_node = leaf;
	it.m_loc = loc3;
	return it;
}

template <typename KeyType, typename DataType, int Dim0, int Dim1, int Dim2>
void L3Tree<KeyType, DataType, Dim0, Dim1, Dim2>::printDetail() const
{
	std::cout << "\n l3 tree root " << m_root.count();
	m_root.printDetail();

	if(m_root.isEmpty()) return;

	InnerNodeType *inner = m_root.value(0);
	for(int i=0;;++i) {
		std::cout << "\n inner " << i;
		inner->printKeyRange();
		inner = inner->rightSibling();
		if(!inner) break;
	}

	std::cout << "\n n data " << size();
}

}

}

#endif
