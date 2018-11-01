/*
 *  HistoryMesh.h
 *
 *  has coarse/fine stages, can sort faces by history
 *
 */

#ifndef ALO_HISTORY_MESH_H
#define ALO_HISTORY_MESH_H

#include <geom/AdaptableMesh.h>
#include "CoarseFineHistory.h"
#include <deque>

namespace alo {


class HistoryMesh : public AdaptableMesh {

    std::deque<CoarseFineHistory> m_stages;
    int m_selectedStage;
    
public:
	HistoryMesh();
	virtual ~HistoryMesh();

	void initHistory();
    void addHistoryStage();
/// coarser ones are in the front
/// stage nf change = nCoarse - nFine
    void finishHistoryStage(const int &nCoarse, const int &nFine);
/// original nv nf
    void finishHistory(int nv, int nf);

/// set nv to coarse at location
    void setHistory(int location);

	void swapHistory(int a, int b);
/// insert n nv at location
	void insertHistory(int n, int location);
/// swap history faces as well
    virtual void swapVertex(int va, int vb,
    			const std::vector<int> &facesa,
    			const std::vector<int> &facesb) override;

    int numStages() const;
    int maxNumStages() const;
    const CoarseFineHistory &stage(int i) const;
    CoarseFineHistory &stage(int i);

/// lod [0,1]
/// stageChanged > 0 increase buffer size
    const CoarseFineHistory &selectStage(int &istage, int &stageChanged, int &nv,
                const float &lod);

    void copyTo(AdaptableMesh *b, const int &nv, const int &nf);

	void printHistoryStage(int i) const;

protected:

private:
    int findStage(const int &nv) const;
};
	
}

#endif
