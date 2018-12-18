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
    ver1::HBase g("/world/meshes");
    
    std::vector<std::string> meshNames;
    g.lsTypedChild<HHistoryMeshRecord>(meshNames);
    const int n = meshNames.size();

    resize(n);
    
    for(int i=0;i<n;++i) {
        std::cout << ".";
        HHistoryMeshRecord hrec(meshNames[i]);
        
        LodMeshCache &c = *m_cacheList[i];
        hrec.load(c.stageDescs());
        c.setLimit();
        c.setMeshName(meshNames[i]);

        hrec.close();

        c.clear();

        c.loadStage(0);
        c.sortCurrentStage();
    }
    
    g.close();

    if(numMeshes() < 1) {
        m_cacheFilePath = "unknown";
        return false;
    }
    
    m_cacheFilePath = fileName;
    return true;
}

void LodMeshGroup::resize(int n)
{
    if(n<1) {
        clear();
        return;
    }

    if(numMeshes() < n)
        expandTo(n);

    if(numMeshes() > n)
        shrinkTo(n);

}

void LodMeshGroup::expandTo(int n)
{
    for(int i=m_cacheList.size();i<n;++i)
        m_cacheList.push_back(new LodMeshCache);
}

void LodMeshGroup::shrinkTo(int n)
{
    for(;;) {
        delete m_cacheList.back();
        m_cacheList.pop_back();

        if(m_cacheList.size() == n)
            return;
    }
}

void LodMeshGroup::getMeshAabb(BoundingBox &box, int i) const
{
    const LodMeshCache *c = m_cacheList[i];
    c->getAabb(box);
}

}
