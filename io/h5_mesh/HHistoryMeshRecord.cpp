#include "HHistoryMeshRecord.h"
#include "HCoarseFineHistory.h"
#include <mesh/CoarseFineHistory.h>
#include <boost/format.hpp>

namespace alo {

HHistoryMeshRecord::HHistoryMeshRecord(const std::string & path) : HHistoryMesh(path)
{}

HHistoryMeshRecord::~HHistoryMeshRecord()
{}

bool HHistoryMeshRecord::load(std::deque<CoarseFineHistoryDesc> &stageDescs)
{
    stageDescs.clear();
    
    int nstg = 0;
    readIntAttr(".nstg", &nstg);
    if(nstg < 1) return false;
    
    for(int i=0;i<nstg;++i) {
        stageDescs.push_back(CoarseFineHistoryDesc());
	    
        const std::string stgName = boost::str(boost::format("stg%1%") % i);
        HCoarseFineHistory fhis(childPath(stgName));
        fhis.load(&stageDescs.back());
	    fhis.close();
    }
    
    return true;
}

}

