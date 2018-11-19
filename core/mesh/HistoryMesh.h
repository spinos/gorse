/*
 *  HistoryMesh.h
 *
 *  coarse/fine history to sort faces in stages
 *
 */

#ifndef ALO_HISTORY_MESH_H
#define ALO_HISTORY_MESH_H

#include <geom/AdaptableMesh.h>
#include "CoarseFineHistory.h"

namespace alo {

class FaceValue;

class HistoryMesh : public AdaptableMesh {

    std::deque<CoarseFineHistory> m_stages;
/// face-varying uv indices
    SimpleBuffer<Int3> m_uvIndices;

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
	
	void initUV();
    void insertUV(const std::vector<int> &faceVertices, 
                const std::deque<FaceValue> &faceUVs, int toFirstFace);
    void swapUV(int fromFace, int toFace);
/// reform uv by indices
    void finishUV();
    
    int numStages() const;
    int maxNumStages() const;
    int minNumVertices() const;
    int minNumTriangles() const;
    int maxNumVertices() const;
    int maxNumTriangles() const;
    const CoarseFineHistory &stage(int i) const;
    CoarseFineHistory &stage(int i);

/// lod [0,1]
    const CoarseFineHistory &selectStage(int &istage, int &nv,
                const float &lod) const;

    void copyStageTo(HistoryMesh *b, int i) const;

	void printHistoryStage(int i) const;

protected:

private:
    int findStage(const int &nv) const;
    int lookupUVIndex(const std::deque<FaceValue> &faceUVs, int vi) const;
/// scatter by uv index
    void reformUV(SimpleBuffer<Float2> &uv);
    
};
	
}

#endif
