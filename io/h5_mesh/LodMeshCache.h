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
class BoundingBox;  
class HistoryMesh;
    
class LodMeshCache : public HistoryReform {

    std::string m_meshName;
    std::deque<CoarseFineHistoryDesc> m_stageDescs;
    HistoryMesh *m_meshInCore;
    int m_minNv;
    int m_maxNv;
    int m_minNt;
    int m_maxNt;
    
public:
    LodMeshCache();
    virtual ~LodMeshCache();

    bool isValid() const;
    const std::string &meshName() const;
    
    bool selectStage(int &istage, int &nv, const float &lod);
    bool selectStageByNv(int &istage, int &nv, const int &lod);
    bool nvChanged(int x) const;
    bool stageChanged(int x) const;
    bool loadStage(int x);
    void reformInCore(AdaptableMesh *outMesh, const int &nv, const int &istage);
    const int &minNumTriangles() const;
    const int &maxNumTriangles() const;
    
    void printStages() const;
    
    std::deque<CoarseFineHistoryDesc> &stageDescs();
    void setMeshName(const std::string &x);
    void setLimit();

    void getAabb(BoundingBox &box) const;
    
protected:

private:
    int findStage(const int &nv) const;
    
};

}

#endif

