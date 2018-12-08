#include "LodMeshCache.h"
#include <iostream>
#include "HHistoryMeshRecord.h"
#include <mesh/HistoryMesh.h>

namespace alo {

LodMeshCache::LodMeshCache() :
m_meshName("unknown"),
m_curStage(-1),
m_curNv(-1)
{
    m_meshInCore = new HistoryMesh;
    m_meshInCore->createTriangleMesh(1000,1000);
/// only stage
    m_meshInCore->addHistoryStage();
}

LodMeshCache::~LodMeshCache()
{ delete m_meshInCore; }

bool LodMeshCache::isValid() const
{
    if(m_meshName=="unknown") return false;
    return true;
}

bool LodMeshCache::selectStage(int &istage, int &nv, const float &lod)
{
    if(!isValid()) return false;
    if(lod <= 0.f) {
		istage = 0;
		nv = m_stageDescs.front()._vbegin;
	} else if(lod >= 1.f) {
		istage = m_stageDescs.size() - 1;
		nv = m_stageDescs.back()._vend;
	} else {
		const int vmin = m_stageDescs.front()._vbegin;
		const int vmax = m_stageDescs.back()._vend;
		nv = vmin + (float)(vmax - vmin + 1) * lod;
		istage = findStage(nv);
	}
    return true;
}

int LodMeshCache::findStage(const int &nv) const
{
	int high = m_stageDescs.size() - 1;
	if(m_stageDescs[high]._vbegin <= nv) return high;
	int low = 0;
	int mid = (low + high)>>1;
	while(high > low+1) {
		const CoarseFineHistoryDesc &d = m_stageDescs[mid];
		if(d._vbegin < nv) low = mid;
		else high = mid;
		mid = (low + high)>>1;
	}
	return low;
}

bool LodMeshCache::stageChanged(int x) const
{ return m_curStage != x; }

bool LodMeshCache::loadStage(int x)
{
    HHistoryMeshRecord hrec(m_meshName);
/// load a stage?
    const CoarseFineHistoryDesc &d = m_stageDescs[x];
    const int nv = d._vend;
    const int nf = d._fend;
    hrec.load(m_meshInCore, nv, nf);

    const int hl = d._length;
    CoarseFineHistory &stg = m_meshInCore->stage(0);
    stg.setDesc(d);
    hrec.load(&stg, hl, x);

    hrec.close();
    
    m_curStage = x;

    sortCoarseFaces(m_meshInCore, 0, stg.coarseMax(), stg.c_value() );
    return true;
}

void LodMeshCache::reformInCore(AdaptableMesh *outMesh, const int &nv, const int &istage)
{
    reform(outMesh, m_meshInCore, nv, istage, m_meshInCore->stage(0) );
}

void LodMeshCache::printStages() const
{
    std::cout << "\n lod mesh n stage " << m_stageDescs.size();
    std::deque<CoarseFineHistoryDesc>::const_iterator it = m_stageDescs.begin();
    int i=0;
    for(;it!=m_stageDescs.end();++it,++i) {
        std::cout << "\n stage" << i << " v [" << it->_vbegin << ": " << it->_vend << "] "
				<< " f [" << it->_fbegin << ": " << it->_fend << "] "
				<< " +coarse " << it->_ncoarse
				<< " -fine " << it->_nfine
				<< " len " << it->_length;
    }
}

void LodMeshCache::setMeshName(const std::string &x)
{ m_meshName = x; }

const std::string &LodMeshCache::meshName() const
{ return m_meshName; }

std::deque<CoarseFineHistoryDesc> &LodMeshCache::stageDescs()
{ return m_stageDescs; }

}
