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
class CoarseFineHistory;
    
class HHistoryMeshRecord : public HHistoryMesh {

public:
    HHistoryMeshRecord(const std::string & path);
    virtual ~HHistoryMeshRecord();
    
    virtual bool load(std::deque<CoarseFineHistoryDesc> &stageDescs);
    virtual bool load(HistoryMesh *msh, int nv, int nt);
    virtual bool load(CoarseFineHistory *stg, int hl, int i);
    
protected:
	bool readF3Data(const std::string &name,
                char *data, const int &count);
	bool readI3Data(const std::string &name,
                char *data, const int &count);

private:

};

}

#endif

