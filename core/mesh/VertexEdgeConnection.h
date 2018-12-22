/*
 *  VertexEdgeConnection.h
 *  aloe
 *
 *  vertex-edge look up
 *
 */
 
#ifndef ALO_VERTEX_EDGE_CONNECTION_H
#define ALO_VERTEX_EDGE_CONNECTION_H

#include "EdgeIndex.h"
#include <vector>
#include <deque>

namespace alo {

class VertexEdgeConnection {
    
    std::vector<std::deque<EdgeIndex> > m_connections;
    
public:
    VertexEdgeConnection();
    virtual ~VertexEdgeConnection();
    
    void resize(int n);
    void connectEdge(const EdgeIndex& e);
    
    const std::deque<EdgeIndex> &operator[](int v) const;
    int totalSize() const;
    
protected:

private:
    
};

}

#endif
