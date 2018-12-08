#include "LodMeshCache.h"
#include <iostream>
#include <h5/V1H5IO.h>
#include <h5/V1HBase.h>
#include "HHistoryMeshRecord.h"
#include <mesh/HistoryMesh.h>
#include <algorithm>

namespace alo {

LodMeshCache::LodMeshCache() :
m_meshName("unknown"),
m_cachePath("unknown"),
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
    return m_cachePath!="unknown";
}

bool LodMeshCache::cachePathChanged(const std::string &x) const
{ return m_cachePath != x; }

bool LodMeshCache::loadCache(const std::string &fileName, const std::string &meshName)
{
    ver1::H5IO hio;
    
    bool stat = hio.begin(fileName);
    if(!stat) {
        m_cachePath = "unknown";
        m_meshName = "unknown";
        return false;
    }
    
    ver1::HBase g("/world/meshes");
    
    std::vector<std::string> meshNames;
    g.lsTypedChild<HHistoryMeshRecord>(meshNames);
    std::vector<std::string>::iterator it;
    
    it = std::find(meshNames.begin(),meshNames.end(), meshName);
    stat = (it != meshNames.end());
    if(stat) {
        HHistoryMeshRecord hrec(meshName);
        hrec.load(m_stageDescs);
        hrec.close();
        m_meshName = meshName;
    }  

    g.close();
    
    hio.end();
    
    m_cachePath = fileName;
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

void LodMeshCache::reformStage(AdaptableMesh *outMesh, const int &nv, const int &istage)
{
    if(stageChanged(istage))
        loadStage(istage);

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

void LodMeshCache::setCacheFilePath(const std::string &x)
{ m_cachePath = x; }

void LodMeshCache::setMeshName(const std::string &x)
{ m_meshName = x; }

const std::string &LodMeshCache::cacheFilePath() const
{ return m_cachePath; }

const std::string &LodMeshCache::meshName() const
{ return m_meshName; }

std::deque<CoarseFineHistoryDesc> &LodMeshCache::stageDescs()
{ return m_stageDescs; }

bool LodMeshCache::Load(std::vector<LodMeshCache *> &dst, const std::string &fileName)
{
    ver1::H5IO hio;
    
    bool stat = hio.begin(fileName);
    if(!stat) return false;
    
    ver1::HBase g("/world/meshes");
    
    std::vector<std::string> meshNames;
    g.lsTypedChild<HHistoryMeshRecord>(meshNames);
    const int n = meshNames.size();
    
    for(int i=0;i<n;++i) {
        HHistoryMeshRecord hrec(meshNames[i]);
        
        if(dst.size() < i + 1) 
            dst.push_back(new LodMeshCache);
        
        LodMeshCache &c = *dst[i];
        hrec.load(c.stageDescs());
        c.setCacheFilePath(fileName);
        c.setMeshName(meshNames[i]);
        
        hrec.close();
    }
    
    g.close();
    
    hio.end();
    
    return true;
}

}
