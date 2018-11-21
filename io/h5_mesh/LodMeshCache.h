#ifndef ALO_LOD_MESH_CACHE_H
#define ALO_LOD_MESH_CACHE_H

#include <string>
#include <deque>
#include <mesh/CoarseFineHistory.h>

namespace alo {
    
class HistoryMesh;
    
class LodMeshCache {

    std::string m_meshName;
    std::string m_cachePath;
    std::deque<CoarseFineHistoryDesc> m_stageDescs;
    HistoryMesh *m_meshInCore;
    int m_curStage;
    int m_curNv;
    
public:
    LodMeshCache();
    virtual ~LodMeshCache();
    
    bool isValid() const;
    
protected:
    bool cachePathChanged(const std::string &x) const;
    bool loadCache(const std::string &x);
    bool selectStage(int &istage, int &nv, const float &lod);
    bool stageChanged(int x) const;
    bool loadStage(int x);
    void printStages() const;
    
private:
    int findStage(const int &nv) const;
    
};

}

#endif

