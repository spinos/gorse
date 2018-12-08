/*
 *  LodMeshCache.h
 *  aloe
 *
 *  mesh_path, reform cache and function
 *
 */

#ifndef ALO_LOD_MESH_CACHE_H
#define ALO_LOD_MESH_CACHE_H

#include <string>
#include <deque>
#include <mesh/CoarseFineHistory.h>
#include <mesh/HistoryReform.h>

namespace alo {
    
class HistoryMesh;
    
class LodMeshCache : public HistoryReform {

    std::string m_meshName;
    std::deque<CoarseFineHistoryDesc> m_stageDescs;
    HistoryMesh *m_meshInCore;
    int m_curStage;
    int m_curNv;
    
public:
    LodMeshCache();
    virtual ~LodMeshCache();

    bool isValid() const;
    const std::string &meshName() const;
    
    bool selectStage(int &istage, int &nv, const float &lod);
    bool stageChanged(int x) const;
    bool loadStage(int x);
    void reformInCore(AdaptableMesh *outMesh, const int &nv, const int &istage);
    
    void printStages() const;
    
    std::deque<CoarseFineHistoryDesc> &stageDescs();
    void setMeshName(const std::string &x);
    
protected:

private:
    int findStage(const int &nv) const;
    
};

}

#endif

