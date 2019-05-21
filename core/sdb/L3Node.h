/*
 *  L3Node.h
 *  sdb
 *
 *  subling -> node -> sibline
 *	pointer to children as data
 *
 */

#ifndef SDB_L3_NODE_H
#define SDB_L3_NODE_H

#include "KeyDataArray.h"

namespace alo {

namespace sdb {

template <typename KeyType, typename DataType, int Dim>
class L3Node : public KeyDataArray<KeyType, DataType, Dim> {

    typedef KeyDataArray<KeyType, DataType, Dim> KDATyp;
    
	L3Node *m_leftSibling;
	L3Node *m_rightSibling;

public:
	L3Node();
	virtual ~L3Node();

	L3Node *spawn();
	void connectToLeftSibling(L3Node *x);
	void connectToRightSibling(L3Node *x);

	L3Node *leftSibling() const;
	L3Node *rightSibling() const;

	void printDetail() const;
	void printKeyRange() const;

protected:

};

template <typename KeyType, typename DataType, int Dim>
L3Node<KeyType, DataType, Dim>::L3Node() :
m_leftSibling(NULL),
m_rightSibling(NULL)
{}

template <typename KeyType, typename DataType, int Dim>
L3Node<KeyType, DataType, Dim>::~L3Node()
{}

template <typename KeyType, typename DataType, int Dim>
void L3Node<KeyType, DataType, Dim>::connectToLeftSibling(L3Node *x)
{ m_leftSibling = x; }

template <typename KeyType, typename DataType, int Dim>
void L3Node<KeyType, DataType, Dim>::connectToRightSibling(L3Node *x)
{ m_rightSibling = x; }

template <typename KeyType, typename DataType, int Dim>
L3Node<KeyType, DataType, Dim> *L3Node<KeyType, DataType, Dim>::leftSibling() const
{ return m_leftSibling; }

template <typename KeyType, typename DataType, int Dim>
L3Node<KeyType, DataType, Dim> *L3Node<KeyType, DataType, Dim>::rightSibling() const
{ return m_rightSibling; }

template <typename KeyType, typename DataType, int Dim>
L3Node<KeyType, DataType, Dim> *L3Node<KeyType, DataType, Dim>::spawn()
{
	L3Node *right = new L3Node;
	int loc = KDATyp::count()>>1; 

	right->copyFrom(&KDATyp::keys()[loc], &KDATyp::values()[loc], KDATyp::count() - loc);
	KDATyp::setCount(loc);
	return right;
}

template <typename KeyType, typename DataType, int Dim>
void L3Node<KeyType, DataType, Dim>::printDetail() const
{
	std::cout << " n " << KDATyp::count() << " [";
	for(int i=0;i<KDATyp::count();++i)
    	std::cout << KDATyp::key(i) << ",";
    std::cout << "]";
}

template <typename KeyType, typename DataType, int Dim>
void L3Node<KeyType, DataType, Dim>::printKeyRange() const
{
	std::cout << " n " << KDATyp::count() << " [";
	if(!KDATyp::isEmpty())
		std::cout << KDATyp::key(0) << ":" << KDATyp::key(KDATyp::count() - 1);
    std::cout << "]";
}

}

}

#endif
