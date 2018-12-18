/*
 *  HistoryMesh.cpp
 *
 *
 */

#include "HistoryMesh.h"
#include "FaceValue.h"

namespace alo {
	
HistoryMesh::HistoryMesh() :
m_cachedStageId(-1)
{}

HistoryMesh::~HistoryMesh()
{ 
	m_stages.clear(); 
}

void HistoryMesh::initHistory()
{
	m_stages.clear();
}

void HistoryMesh::initUV()
{
	const int nf = numTriangles();
	m_uvIndices.createBuffer(nf);
	for(int i=0;i<nf;++i)
		m_uvIndices[i].set(i * 3, i * 3 + 1, i * 3 + 2);
}

void HistoryMesh::swapUV(int fromFace, int toFace)
{ m_uvIndices.swap(fromFace, toFace, 1); }

void HistoryMesh::insertUV(const std::vector<int> &faceVertices, 
                            const std::deque<FaceValue> &faceUVs, int toFirstFace)
{
    const int nv = faceVertices.size();
    const int nf = nv / 3;
    Int3 *b = new Int3[nf];
    int *bi = (int *)b;
    for(int i=0;i<nv;++i)
        bi[i] = lookupUVIndex(faceUVs, faceVertices[i]);
    
    m_uvIndices.insert(b, nf, toFirstFace);
    delete[] b;
}

int HistoryMesh::lookupUVIndex(const std::deque<FaceValue> &faceUVs, int vi) const
{
    std::deque<FaceValue>::const_iterator it = faceUVs.begin();
    for(;it!=faceUVs.end();++it) {
        int uv = it->vertexUV(vi);
        if(uv > -1) return uv;
    }
    return -1;
}

void HistoryMesh::finishUV()
{
    for(int i=0;i<numUVSets();++i)
        reformUV(uvBuffer(i));
}

void HistoryMesh::reformUV(SimpleBuffer<Float2> &uv )
{
    const SimpleBuffer<Float2> b(uv);
    const int nf = m_uvIndices.count();
    const int nfv = nf * 3;
    uv.resetBuffer(nfv);
    
    const Int3 *ind = m_uvIndices.c_data();
    for(int i=0;i<nf;++i) {
        const Int3 &fi = ind[i]; //std::cout << " (" << fi.x <<","<<fi.y<<","<<fi.z<<")";
        uv[i*3    ] = b[fi.x];
        uv[i*3 + 1] = b[fi.y];
        uv[i*3 + 2] = b[fi.z];
    }
}

void HistoryMesh::addHistoryStage()
{
	m_stages.push_front(CoarseFineHistory());
	m_stages.front().initialize(numTriangles());
}

void HistoryMesh::finishHistoryStage(const int &nCoarse, const int &nFine)
{ 
    CoarseFineHistory &his = m_stages[0];
    his.setVCF(numVertices(), nCoarse, nFine);
}

void HistoryMesh::setHistory(int location)
{ 
/// in current stage only
	//if(m_fineHistory[location] != 1<<30) {
	//	std::cout<<"\n\n WARNING history overwrite" <<m_fineHistory[location]
	//		<< " to " << numVertices(); 
	//}
	CoarseFineHistory &his = m_stages[0];
	his.setNv(numVertices(), location ); 
}

void HistoryMesh::swapHistory(int a, int b)
{
/// in all stages
	for(int i=0;i<numStages();++i) {
		CoarseFineHistory &his = m_stages[i];
    	his.swap(a, b);
    }
}

void HistoryMesh::insertHistory(int n, int location)
{
/// to all stages
	const int &nv = numVertices();
	for(int i=0;i<numStages();++i) {
    	CoarseFineHistory &his = m_stages[i];
    	his.insert(nv, n, location);
	}
}

void HistoryMesh::finishHistory(int nv, int nf)
{
	int stageEndHh = m_stages.back().length();

	for(int i =numStages()-1;i>=0;--i) {
		CoarseFineHistory &stg = m_stages[i];

		int nvred;
		if(i==numStages() - 1) {
			stg.vend() = nv;
		}
		else {
			stg.vend() = m_stages[i+1].vbegin();
		}

		stg.setFRange(stageEndHh, stageEndHh - stg.nfine() );

		stageEndHh = stg.fbegin();
	}	
}

int HistoryMesh::numStages() const
{ return m_stages.size(); }

const CoarseFineHistory &HistoryMesh::stage(int i) const
{ return m_stages[i]; }

CoarseFineHistory &HistoryMesh::stage(int i)
{ return m_stages[i]; }

int HistoryMesh::maxNumStages() const
{ return 9; }

int HistoryMesh::minNumVertices() const
{ return m_stages.front().vbegin(); }

int HistoryMesh::minNumTriangles() const
{ return m_stages.front().fbegin(); }

int HistoryMesh::maxNumVertices() const
{ return m_stages.back().vend(); }

int HistoryMesh::maxNumTriangles() const
{ return m_stages.back().fend(); }

const CoarseFineHistory &HistoryMesh::selectStage(int &istage, int &nv,
                const float &lod) const
{
	if(lod <= 0.f) {
		istage = 0;
		nv = m_stages.front().vbegin();
	} else if(lod >= 1.f) {
		istage = numStages() - 1;
		nv = m_stages.back().vend();
	} else {
		const int vmin = m_stages.front().vbegin();
		const int vmax = m_stages.back().vend();
		nv = vmin + (float)(vmax - vmin + 1) * lod;
		istage = findStage(nv);
	}
	return m_stages[istage];
}

const CoarseFineHistory &HistoryMesh::selectStageByVertexCount(int &istage, int &nv,
                const int &vcount) const
{
	const int vmin = m_stages.front().vbegin();
	const int vmax = m_stages.back().vend();
	if(vcount <= vmin) {
		istage = 0;
		nv = vmin;
	} else if(vcount >= vmax) {
		istage = numStages() - 1;
		nv = vmax;
	} else {
		nv = vcount;
		istage = findStage(nv);
	}
	return m_stages[istage];
}


int HistoryMesh::findStage(const int &nv) const
{
	int high = numStages() - 1;
	if(m_stages[high].vbegin() <= nv) return high;
	int low = 0;
	while(high > low+1) {
		int mid = (low + high)>>1;
		const CoarseFineHistory &his = m_stages[mid];
		if(his.vbegin() < nv) low = mid;
		else high = mid;
	}
	return low;
}

void HistoryMesh::copyStageTo(HistoryMesh *b, int i) const
{
	int nv = m_stages[i].vend();
	int nf = m_stages[i].fend();
	copyMeshTo(b, nv, nf);
	b->stage(0) = m_stages[i];
	b->setCachedStageId(i);
}

const int &HistoryMesh::cachedStageId() const
{ return m_cachedStageId; }

void HistoryMesh::setCachedStageId(int x)
{ m_cachedStageId = x; }

void HistoryMesh::printHistoryStage(int i) const
{ 
	std::cout << "\n stage "<<i;
	m_stages[i].printDetail();
}

}
