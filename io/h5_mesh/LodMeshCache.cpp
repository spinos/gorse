#include "LodMeshCache.h"
#include <iostream>
#include "HHistoryMeshRecord.h"
#include <mesh/HistoryMesh.h>

namespace alo {

LodMeshCache::LodMeshCache() :
m_meshName("unknown"),
m_currentCacheId(-1)
{
    m_outMesh = new AdaptableMesh;
    for(int i=0;i<3;++i) {
        m_historyInCore[i] = new HistoryMesh;
        m_historyInCore[i]->createTriangleMesh(1000,1000);
/// only stage
        m_historyInCore[i]->addHistoryStage();
    }
}

LodMeshCache::~LodMeshCache()
{ 
    delete m_outMesh;
    for(int i=0;i<3;++i)
        delete m_historyInCore[i]; 
}

void LodMeshCache::clear()
{ 
    m_currentCacheId = -1; 
    for(int i=0;i<3;++i) {
        m_historyInCore[i]->setCachedStageId(-1);
        m_historyInCore[i]->purgeMesh();
    }
}

bool LodMeshCache::isValid() const
{
    if(m_meshName=="unknown") return false;
    return true;
}

void LodMeshCache::setLimit()
{
    m_minNv = m_stageDescs.front()._vbegin;
    m_minNt = m_stageDescs.front()._fbegin;
    m_maxNv = m_stageDescs.back()._vend;
    m_maxNt = m_stageDescs.back()._fend;
}

int LodMeshCache::selectStage(int &nv, const float &lod) const
{
    int istage;
    if(lod <= 0.f) {
		istage = 0;
		nv = m_minNv;
	} else if(lod >= 1.f) {
		istage = m_stageDescs.size() - 1;
		nv = m_maxNv;
	} else {
		nv = m_minNv + (float)(m_maxNv - m_minNv) * lod;
		istage = findStage(nv);
	}
    return istage;
}

int LodMeshCache::selectStageByNv(int &nv) const
{
    int istage;
    if(nv <= m_minNv) {
        istage = 0;
        nv = m_minNv;
    } else if(nv >= m_maxNv) {
        istage = m_stageDescs.size() - 1;
        nv = m_maxNv;
    } else {
        istage = findStage(nv);
    }
    return istage;
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

const int &LodMeshCache::minNumTriangles() const
{ return m_minNt; }

const int &LodMeshCache::maxNumTriangles() const
{ return m_maxNt; }

bool LodMeshCache::loadStage(int x)
{
    HistoryMesh *meshInCore = selectCurrentCache(x);
    HHistoryMeshRecord hrec(m_meshName);

    const CoarseFineHistoryDesc &d = m_stageDescs[x];
    const int nv = d._vend;
    const int nf = d._fend;
    hrec.load(meshInCore, nv, nf);

    const int hl = d._length;
    CoarseFineHistory &stg = meshInCore->stage(0);
    stg.setDesc(d);
    hrec.load(&stg, hl, x);

    hrec.close();

    meshInCore->setCachedStageId(x);

    return true;
}

void LodMeshCache::sortCurrentStage()
{
    HistoryMesh *meshInCore = currentCache();
    CoarseFineHistory &stg = meshInCore->stage(0);
    sortCoarseFaces(meshInCore, 0, stg.coarseMax(), stg.c_value() );
}

void LodMeshCache::reformInCore(const int &nv)
{
    HistoryMesh *meshInCore = currentCache();
    reform(m_outMesh, meshInCore, nv, meshInCore->cachedStageId(), meshInCore->stage(0) );
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

void LodMeshCache::getAabb(BoundingBox &box) const
{ c_currentCache()->getAabb(box); }

const AdaptableMesh *LodMeshCache::c_outMesh() const
{ return m_outMesh; }

HistoryMesh *LodMeshCache::currentCache()
{ return m_historyInCore[m_currentCacheId]; }

const HistoryMesh *LodMeshCache::c_currentCache() const
{ return m_historyInCore[m_currentCacheId]; }

bool LodMeshCache::switchToStage(int x)
{
    if(m_currentCacheId < 0) return false;
    if(currentCache()->cachedStageId() == x) return true;
    for(int i=0;i<3;++i) {
        if(m_historyInCore[i]->cachedStageId() == x) {
            m_currentCacheId = i;
            //sortCurrentStage();
            return true;
        }
    }
    return false;
}

HistoryMesh *LodMeshCache::selectCurrentCache(int x)
{
    int selC = 0; 
    int md = 0;
    for(int i=0;i<3;++i) {
        const int ci = m_historyInCore[i]->cachedStageId();
        if(ci < 0) {
            selC = i;
            break;
        }
        int d = ci - x;
        if(d < 0) d = -d;
        if(md < d) {
            md = d;
            selC = i;
        }
    }
    
    m_currentCacheId = selC;
    return m_historyInCore[m_currentCacheId];
}

bool LodMeshCache::isMeshCached(const int &x) const
{ return m_outMesh->numVertices() == x; }

}
