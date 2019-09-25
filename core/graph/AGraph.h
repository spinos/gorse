/*
 *  AGraph.h
 *  aloe
 *
 *  Graph system with nodes and edges
 *  per-node edge indices like 0 1 2 3 0 3 4 ...
 *  per-node edge offsets like 0 4 7 ...
 *  
 *  2019/9/25
 */

#ifndef ALO_GRAPH_H
#define ALO_GRAPH_H

#include <sdb/Types.h>
#include <sdb/L3Tree.h>
#include <vector>
#include <iostream>

namespace alo {

template<typename Tn, typename Te>
class AGraph {

	Te * m_edges;
	int * m_vvEdgeIndices;
	int * m_vvEdgeBegins;
	Tn * m_nodes;
	int m_numNode, m_numEdge, m_numEdgeInd;
	
public:

	AGraph();
	~AGraph();
	
	void create(int nn, int ne, int ni);
	
	const int & numNodes() const;
	const int & numEdges() const;
	const int & numEdgeIndices() const;
	
	Tn * nodes();
	Te * edges();
	int * edgeIndices();
	int * edgeBegins();
	
	const Tn * nodes() const;
	const Te * edges() const;
	const int * edgeIndices() const;
	const int * edgeBegins() const;
	
	const Te * edge(const int & v1, const int & v2) const;
	Te * edge(const int & v1, const int & v2);
/// opposite to v1 on ej-th edge	
	int oppositeNodeIndex(const int& v1, const int& ej) const;
	
	void verbose() const;
	
	typedef Tn NodeType;
	typedef Te EdgeType;
    
	void extractEdges(sdb::L3Tree<sdb::Coord2, int, 2048, 512, 1024> *a);
	void extractEdgeBegins(const std::vector<int> & a);
	void extractEdgeIndices(const std::vector<int> & a);
	
protected:
/// ind to edge by vertex i
	int edgeIndex(const int & v1, const int & v2) const;
	
private:
	void internalClear();

};

template<typename Tn, typename Te>
AGraph<Tn, Te>::AGraph() : 
m_edges(0),
m_vvEdgeIndices(0),
m_vvEdgeBegins(0),
m_nodes(0),
m_numNode(0),
m_numEdge(0),
m_numEdgeInd(0)
{}

template<typename Tn, typename Te>
AGraph<Tn, Te>::~AGraph()
{ internalClear(); }

template<typename Tn, typename Te>
void AGraph<Tn, Te>::internalClear()
{
	if(m_edges) delete[] m_edges;
	if(m_vvEdgeIndices) delete[] m_vvEdgeIndices;
	if(m_vvEdgeBegins) delete[] m_vvEdgeBegins;
	if(m_numNode > 0 && m_nodes) delete[] m_nodes;
}

template<typename Tn, typename Te>
void AGraph<Tn, Te>::create(int nn, int ne, int ni)
{
	internalClear();
	m_numNode = nn;
	m_numEdge = ne;
	m_numEdgeInd = ni;
	m_nodes = new Tn[m_numNode];
	m_edges = new Te[m_numEdge];
	m_vvEdgeBegins = new int[m_numNode + 1];
	m_vvEdgeBegins[m_numNode] = m_numEdgeInd;
	m_vvEdgeIndices = new int[m_numEdgeInd];
}

template<typename Tn, typename Te>
const int & AGraph<Tn, Te>::numNodes() const
{ return m_numNode; }
	
template<typename Tn, typename Te>
const int & AGraph<Tn, Te>::numEdges() const
{ return m_numEdge; }

template<typename Tn, typename Te>
const int & AGraph<Tn, Te>::numEdgeIndices() const
{ return m_numEdgeInd; }

template<typename Tn, typename Te>
Tn * AGraph<Tn, Te>::nodes()
{ return m_nodes; }

template<typename Tn, typename Te>
Te * AGraph<Tn, Te>::edges()
{ return m_edges; }

template<typename Tn, typename Te>
int * AGraph<Tn, Te>::edgeIndices()
{ return m_vvEdgeIndices; }

template<typename Tn, typename Te>
int * AGraph<Tn, Te>::edgeBegins()
{ return m_vvEdgeBegins; }

template<typename Tn, typename Te>
const Tn * AGraph<Tn, Te>::nodes() const
{ return m_nodes; }

template<typename Tn, typename Te>
const Te * AGraph<Tn, Te>::edges() const
{ return m_edges; }

template<typename Tn, typename Te>
const int * AGraph<Tn, Te>::edgeIndices() const
{ return m_vvEdgeIndices; }

template<typename Tn, typename Te>
const int * AGraph<Tn, Te>::edgeBegins() const
{ return m_vvEdgeBegins; }

template<typename Tn, typename Te>
void AGraph<Tn, Te>::extractEdges(sdb::L3Tree<sdb::Coord2, int, 2048, 512, 1024> * a)
{
	int offset = 0;
	sdb::L3Node<sdb::Coord2, int, 1024> *block = a->begin();
	while(block) {
		for (int i=0;i<block->count();++i) { 
		    Te* e = &m_edges[offset];
		    e->vi = block->key(i);
		
		    offset++;
		}
		block = a->next(block);
	}
}

template<typename Tn, typename Te>
void AGraph<Tn, Te>::extractEdgeBegins(const std::vector<int> & a)
{
	int i = 0;
	std::vector<int>::const_iterator it = a.begin();
	for(;it!=a.end();++it) {
		m_vvEdgeBegins[i++] = *it;
	}
}
 
template<typename Tn, typename Te>
void AGraph<Tn, Te>::extractEdgeIndices(const std::vector<int> & a)
{
	int i = 0;
	std::vector<int>::const_iterator it = a.begin();
	for(;it!=a.end();++it) {
		m_vvEdgeIndices[i++] = *it;
	}
}	
	
template<typename Tn, typename Te>
int AGraph<Tn, Te>::edgeIndex(const int & v1, const int & v2) const
{
	if(numEdgeIndices() < 1) return -1;
	if(v1>= numNodes() ) {
		std::cout<<"\n AGraph invalid node ind "<<v1;
		return -1;
	}
	if(v2>= numNodes() ) {
		std::cout<<"\n AGraph invalid node ind "<<v2;
		return -1;
	}
	
	// const Tn & A = nodes()[v1];
	const int endj = edgeBegins()[v1+1];
	int j = edgeBegins()[v1];
	for(;j<endj;++j) {
		
		if(j>numEdgeIndices()-1 ) {
			std::cout<<"\n AGraph invalid edge ind "<<j;
		}
		
		int k = edgeIndices()[j];
		
		if(k>numEdges()-1 ) {
			std::cout<<"\n AGraph invalid edge i "<<k;
		}

		const Te & eg = edges()[k];
		
		if(eg.vi.x == v2 || eg.vi.y == v2)
			return k;
	}
	return -1;
}

template<typename Tn, typename Te>
const Te * AGraph<Tn, Te>::edge(const int & v1, const int & v2) const
{ 
	int k = edgeIndex(v1, v2); 
	if(k<0) {
		//std::cout<<"\n invalid edge ("<<v1<<","<<v2<<")"<<std::endl;
		return NULL;
	}
		
	return &edges()[k];
}

template<typename Tn, typename Te>
Te * AGraph<Tn, Te>::edge(const int & v1, const int & v2)
{ 
	int k = edgeIndex(v1, v2); 
	if(k<0) {
		//std::cout<<"\n invalid edge ("<<v1<<","<<v2<<")"<<std::endl;
		return NULL;
	}
		
	return &edges()[k];
}

template<typename Tn, typename Te>
int AGraph<Tn, Te>::oppositeNodeIndex(const int& v1, const int& ej) const
{
    const Te & eg = edges()[ej];
    int v2 = eg.vi.x;
    if(v2 == v1) {
        v2 = eg.vi.y;
    }
    return v2;
}

template<typename Tn, typename Te>
void AGraph<Tn, Te>::verbose() const
{
	std::cout<<"\n graph n node "<<numNodes()
			<<"  n edge "<<numEdges()
			<<"  n node-edge "<<numEdgeIndices();
}

}
#endif
