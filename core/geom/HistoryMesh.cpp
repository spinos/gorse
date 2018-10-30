/*
 *  HistoryMesh.cpp
 *
 *
 */

#include "HistoryMesh.h"

namespace alo {
	
HistoryMesh::HistoryMesh()
{}

HistoryMesh::~HistoryMesh()
{}

void HistoryMesh::initHistory()
{
	m_historyLength = numTriangles();
	m_history.createBuffer(m_historyLength);
	static const int unn = 1<<30;
	for(int i=0;i<m_historyLength;++i)
		m_history[i] = unn;
}

void HistoryMesh::swapHistory(int a, int b)
{
	m_history[a] = numVertices();
	m_history.swap(a, b, 1);
}

void HistoryMesh::insertHistory(int n, int location)
{
	int *b = new int[n];
    for(int i=0;i<n;++i) {
        b[i] = numVertices();
    }
    m_history.insert(b, n, location);
    delete[] b;
    m_historyLength += n;
}

void HistoryMesh::sortFaces()
{
	
}

void HistoryMesh::printHistory() const
{ 
	std::cout << "\n history n " << m_historyLength << " (";
	for(int i=0;i<m_historyLength;++i)
		std::cout << " " << m_history[i];
	std::cout << " ) ";
}

}
