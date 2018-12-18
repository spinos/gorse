#include "HHistoryMeshRecord.h"
#include "HCoarseFineHistory.h"
#include "HFaceVaryingUV.h"
#include <mesh/CoarseFineHistory.h>
#include <mesh/HistoryMesh.h>
#include <boost/format.hpp>
#include <h5/HOocArray.h>
#include <math/roundi.h>

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

bool HHistoryMeshRecord::load(HistoryMesh *msh, int nv, int nt)
{
    msh->createTriangleMesh(nv, nt);

    readF3Data(".pos", (char *)msh->positions(), nv);
    readF3Data(".nml", (char *)msh->normals(), nv);
    readI3Data(".ind", (char *)msh->indices(), nt);

    int nuvs = 0;

    readIntAttr(".nuvs", &nuvs);

    if(nuvs < 1) {
        msh->clearUVSets();
        return true;
    }

    msh->createUVSets(nuvs);

    HFaceVaryingUV fuv(childPath("face_varying_uv"));
    fuv.load(msh, nt);
    fuv.close();

    return true;
}

bool HHistoryMeshRecord::load(CoarseFineHistory *stg, int hl, int i)
{
    stg->createHistory(hl);
    const std::string stgName = boost::str(boost::format("stg%1%") % i);
    HCoarseFineHistory fhis(childPath(stgName));
    fhis.load(stg, hl);
    fhis.close();
    return true;
}

bool HHistoryMeshRecord::readF3Data(const std::string &name,
                char *data, const int &count)
{
    typedef HOocArray<hdata::TFloat, 96, 96> F3Data;
    bool stat;
    F3Data *vec = openDataStorage<F3Data>(name, stat);
    if(!stat) {
        std::cout << "\n ERROR cannot open ooc storage "<<name;
        return false;
    }
    
    int nc = Round32(count)>>5;
    
    vec->readColumns(data, 0, nc);
    delete vec;
    return true;
}

bool HHistoryMeshRecord::readI3Data(const std::string &name,
                char *data, const int &count)
{
    typedef HOocArray<hdata::TInt, 96, 96> I3Data;
    bool stat;
    I3Data *ind = openDataStorage<I3Data>(name, stat);
    if(!stat) {
        std::cout << "\n ERROR cannot open ooc storage "<<name;
        return false;
    }
    
    int nc = Round32(count)>>5;
    
    ind->readColumns(data, 0, nc);
    delete ind;
    return true;
}

}
