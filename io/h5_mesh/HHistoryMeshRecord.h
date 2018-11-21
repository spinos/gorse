/*
 *  HHistoryMeshRecord.h
 *  aloe
 *
 *  ooc read on demand
 *
 */
#ifndef ALO_H5_HISTORY_MESH_RECORD_H
#define ALO_H5_HISTORY_MESH_RECORD_H 

#include "HHistoryMesh.h"
#include <deque>

namespace alo {

struct CoarseFineHistoryDesc;
    
class HHistoryMeshRecord : public HHistoryMesh {

public:
    HHistoryMeshRecord(const std::string & path);
    virtual ~HHistoryMeshRecord();
    
    virtual bool load(std::deque<CoarseFineHistoryDesc> &stageDescs);
    
protected:

private:

};

}

#endif

