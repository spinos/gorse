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
{ 
	m_stages.clear(); 
}

void HistoryMesh::initHistory()
{
	m_stages.clear();
}

void HistoryMesh::initUV()
{
	const int nfv = numTriangles() * 3;
	m_uvIndices.createBuffer(nfv);
	for(int i=0;i<nfv;++i)
		m_uvIndices[i] = i;
}

void HistoryMesh::finishUV()
{
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

void HistoryMesh::swapVertex(int va, int vb,
    			const std::vector<int> &facesa,
    			const std::vector<int> &facesb)
{
	AdaptableMesh::swapVertex(va, vb, facesa, facesb);
#if 0
/// last stage is the longest
	const int nf = m_stages.back().length();
	for(int i=numTriangles(); i<nf;++i) {
		unsigned *fi = &indices()[i*3];
		if(fi[0] == vb) {
			fi[0] = va;
		} else if(fi[0] == va) {
			fi[0] = vb;
		}
		if(fi[1] == vb) {
			fi[1] = va;
		} else if(fi[1] == va) {
			fi[1] = vb;
		}
		if(fi[2] == vb) {
			fi[2] = va;
		} else if(fi[2] == va) {
			fi[2] = vb;
		}
	}
#endif
}

void HistoryMesh::insertFaces(const std::vector<int> &faceVertices, int toFirstFace)
{
	AdaptableMesh::insertFaces(faceVertices, toFirstFace);
/// todo insert uv inds
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

	for(int i =0;i<numStages();++i) 
		std::cout << "\n stage"<<i<<stage(i);
	
	std::cout<<"\n n vertex "<<numVertices()
				<< " n face "<<numTriangles();

}

int HistoryMesh::numStages() const
{ return m_stages.size(); }

const CoarseFineHistory &HistoryMesh::stage(int i) const
{ return m_stages[i]; }

CoarseFineHistory &HistoryMesh::stage(int i)
{ return m_stages[i]; }

int HistoryMesh::maxNumStages() const
{ return 8; }

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

int HistoryMesh::findStage(const int &nv) const
{
	int high = numStages() - 1;
	if(m_stages[high].vbegin() <= nv) return high;
	int low = 0;
	int mid = (low + high)>>1;
	while(high > low+1) {
		const CoarseFineHistory &his = m_stages[mid];
		if(his.vbegin() < nv) low = mid;
		else high = mid;
		mid = (low + high)>>1;
	}
	return low;
}

void HistoryMesh::copyTo(AdaptableMesh *b, const int &nv, const int &nf) const
{
	b->createTriangleMesh(c_indices(), 
		c_positions(), c_normals(),
		 nv, nf);
}

void HistoryMesh::copyStageTo(HistoryMesh *b, int i) const
{
	int nv = m_stages[i].vend();
	int nf = m_stages[i].fend();
	copyTo(b, nv, nf);
//	b->initHistory();
/// only 1 stage
//	b->addHistoryStage();
	b->stage(0) = m_stages[i];
}

void HistoryMesh::printHistoryStage(int i) const
{ 
	std::cout << "\n stage "<<i;
	m_stages[i].printDetail();
}

}
