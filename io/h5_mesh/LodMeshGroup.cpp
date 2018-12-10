/*
 *  LodMeshGroup.cpp
 *  aloe
 *
 */

#include "LodMeshGroup.h"
#include "LodMeshCache.h"
#include "HHistoryMesh.h"
#include "HHistoryMeshRecord.h"
#include <h5/V1HBase.h>

namespace alo {

LodMeshGroup::LodMeshGroup() :
m_cacheFilePath("unknown")
{}

LodMeshGroup::~LodMeshGroup()
{ clear(); }

void LodMeshGroup::clear()
{
	std::vector<LodMeshCache *>::iterator it = m_cacheList.begin();
	for(;it!=m_cacheList.end();++it)
		delete *it;
	m_cacheList.clear();
}

const std::string &LodMeshGroup::cacheFilePath() const
{ return m_cacheFilePath; }

int LodMeshGroup::numMeshes() const
{ return m_cacheList.size(); }

LodMeshCache *LodMeshGroup::mesh(int i)
{ return m_cacheList[i]; }

bool LodMeshGroup::loadMeshes(const std::string &fileName)
{
    clear();
    
    ver1::HBase g("/world/meshes");
    
    std::vector<std::string> meshNames;
    g.lsTypedChild<HHistoryMeshRecord>(meshNames);
    const int n = meshNames.size();
    
    for(int i=0;i<n;++i) {
        std::cout << ".";
        HHistoryMeshRecord hrec(meshNames[i]);
        
        if(m_cacheList.size() < i + 1) 
            m_cacheList.push_back(new LodMeshCache);
        
        LodMeshCache &c = *m_cacheList[i];
        hrec.load(c.stageDescs());
        c.setMeshName(meshNames[i]);
        
        hrec.close();
    }
    
    g.close();

    if(numMeshes() < 1) {
        m_cacheFilePath = "unknown";
        return false;
    }
    
    m_cacheFilePath = fileName;
    return true;
}

}
