/*
 *  HistoryMesh.cpp
 *
 *
 */

#include "HistoryMesh.h"

namespace alo {
	
HistoryMesh::HistoryMesh() : m_numStages(0)
{}

HistoryMesh::~HistoryMesh()
{}

void HistoryMesh::initHistory()
{
	m_historyLength = numTriangles();
	
	static const int unn = 1<<30;
		
	m_coarseHistory.createBuffer(m_historyLength);
	for(int i=0;i<m_historyLength;++i)
		m_coarseHistory[i] = unn;
	
	m_fineHistory.createBuffer(m_historyLength);
	for(int i=0;i<m_historyLength;++i)
		m_fineHistory[i] = unn;
}

void HistoryMesh::setCoarseHistory(int location)
{ m_coarseHistory[location] = numVertices(); }

void HistoryMesh::setFineHistory(int location)
{ m_fineHistory[location] = numVertices(); }

void HistoryMesh::swapHistory(int a, int b)
{
	m_coarseHistory.swap(a, b, 1);
	m_fineHistory.swap(a, b, 1);
}

void HistoryMesh::insertHistory(int n, int location)
{
	int *b = new int[n];
    for(int i=0;i<n;++i) {
        b[i] = numVertices();
    }
    m_coarseHistory.insert(b, n, location);
    m_fineHistory.insert(b, n, location);
    delete[] b;
    m_historyLength += n;
}

void HistoryMesh::setHistoryBegin(int n, int istage)
{ 
    m_stageBegins[istage] = n; 
    m_numStages = n + 1;
}

void HistoryMesh::printHistory() const
{ 
	std::cout << "\n history n " << m_historyLength 
	    << " stage0 " << m_stageBegins[0] << " [";
#if 0
	static const int unn = 1<<29;
#if 0
	for(int i=0;i<m_stageBegins[0];++i) {
	    const int & vi = m_coarseHistory[i];
#else
    for(int i=m_stageBegins[0];i<m_historyLength;++i) {
        const int & vi = m_fineHistory[i];
#endif
		if(vi > unn)
		    std::cout << "." ;
	    else 
	        std::cout << " " << vi;
	}
#endif
	std::cout << "] ";
}

}
