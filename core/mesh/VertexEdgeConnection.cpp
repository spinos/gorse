/*
 *  VertexEdgeConnection.cpp
 *  aloe
 *
 */
 
#include "VertexEdgeConnection.h"

namespace alo {

VertexEdgeConnection::VertexEdgeConnection()
{}

VertexEdgeConnection::~VertexEdgeConnection()
{}

void VertexEdgeConnection::resize(int n)
{ m_connections.resize(n); }

void VertexEdgeConnection::connectEdge(const EdgeIndex& e)
{
    m_connections[e.v0()].push_back(e);
    m_connections[e.v1()].push_back(e);
}

const std::deque<EdgeIndex> &VertexEdgeConnection::operator[](int v) const
{ return m_connections[v]; }

int VertexEdgeConnection::totalSize() const
{
    int s = 0;
	std::vector<std::deque<EdgeIndex> >::const_iterator it = m_connections.begin();
	for(;it!=m_connections.end();++it)
		s += it->size();
	return s;
}

}
