/*
 *  HistoryMesh.h
 *
 *  history to reform faces
 *
 */

#ifndef ALO_HISTORY_MESH_H
#define ALO_HISTORY_MESH_H

#include "AdaptableMesh.h"

namespace alo {

class HistoryMesh : public AdaptableMesh {

	SimpleBuffer<int> m_history;
    int m_historyLength;

public:
	HistoryMesh();
	virtual ~HistoryMesh();

	void initHistory();

/// set nv at a swap to b
	void swapHistory(int a, int b);
/// insert n nv at location
	void insertHistory(int n, int location);
	void sortFaces();
	void printHistory() const;

protected:

private:

};
	
}

#endif
