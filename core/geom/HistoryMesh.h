/*
 *  HistoryMesh.h
 *
 *  history to reform faces
 *  coarse and fine pair for each face
 *  fine --------->
 *             n-1,n,n+1
 *  coase          <-------
 *             n+1,n,n-1
 *  n is selected num vertices
 *  fine is increased
 *  coarse is decreased when more n is selected
 *
 */

#ifndef ALO_HISTORY_MESH_H
#define ALO_HISTORY_MESH_H

#include "AdaptableMesh.h"

namespace alo {

class HistoryMesh : public AdaptableMesh {

	SimpleBuffer<int> m_fineHistory;
	SimpleBuffer<int> m_coarseHistory;
    int m_historyLength;
    int m_stageBegins[17];
    int m_numStages;
    
public:
	HistoryMesh();
	virtual ~HistoryMesh();

	void initHistory();

/// set nv to coarse at location
    void setCoarseHistory(int location);
    void setFineHistory(int location);
	void swapHistory(int a, int b);
/// insert n nv at location
	void insertHistory(int n, int location);
/// coarse end and fine begin of i-th stage
    void setHistoryBegin(int n, int istage);

	void printHistory() const;

protected:

private:

};
	
}

#endif
