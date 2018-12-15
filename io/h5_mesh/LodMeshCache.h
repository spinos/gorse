/*
 *  LodMeshCache.h
 *  aloe
 *
 *  history cache and reform function
 *  two in-core stage to reduce read
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
class AdaptableMesh;
class HistoryMesh;
    
class LodMeshCache : public HistoryReform {

    std::string m_meshName;
    std::deque<CoarseFineHistoryDesc> m_stageDescs;
    AdaptableMesh *m_outMesh;
    HistoryMesh *m_historyInCore[2];
    int m_currentCacheId;
    int m_minNv;
    int m_maxNv;
    int m_minNt;
    int m_maxNt;
    
public:
    LodMeshCache();
    virtual ~LodMeshCache();

    bool isValid() const;
    const std::string &meshName() const;
    const int &minNumTriangles() const;
    const int &maxNumTriangles() const;
    
    int selectStage(int &nv, const float &lod) const;
    int selectStageByNv(int &nv) const;

/// false if stage x is not cached
    bool switchToStage(int x);
    bool nvChanged(int x) const;

/// hdf5 read cannot parallel
    bool loadStage(int x);
    void sortCurrentStage();
    void reformInCore(const int &nv, const int &istage);
    
    void printStages() const;
    
    std::deque<CoarseFineHistoryDesc> &stageDescs();
    void setMeshName(const std::string &x);
    void setLimit();

    void getAabb(BoundingBox &box) const;

    AdaptableMesh *c_outMesh() const;
    
protected:

private:
    int findStage(const int &nv) const;
    HistoryMesh *currentCache();
    const HistoryMesh *c_currentCache() const;
/// empty or furthest from x
    HistoryMesh *selectCurrentCache(int x);
    
};

}

#endif

