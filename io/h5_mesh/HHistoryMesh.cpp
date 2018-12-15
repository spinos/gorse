/*
 *  HHistoryMesh.cpp
 *  aloe
 *
 *
 */

#include "HHistoryMesh.h"
#include <vector>
#include <sstream>
#include <iostream>
#include <mesh/HistoryMesh.h>
#include <h5/HOocArray.h>
#include <math/roundi.h>
#include "HFaceVaryingUV.h"
#include "HCoarseFineHistory.h"
#include <boost/format.hpp>

namespace alo {

HHistoryMesh::HHistoryMesh(const std::string & path) : ver1::HBase(path) 
{}

HHistoryMesh::~HHistoryMesh() {}

bool HHistoryMesh::verifyType()
{
	if(!hasNamedAttr(".ntri"))
		return false;

	if(!hasNamedAttr(".nv"))
		return false;

	if(!hasNamedAttr(".nstg"))
		return false;

	if(!hasNamedAttr(".nuvs"))
		return false;
	
	return true;
}

bool HHistoryMesh::save(const HistoryMesh *msh)
{
	int nv = msh->maxNumVertices();
	if(!hasNamedAttr(".nv"))
		addIntAttr(".nv");
	
	writeIntAttr(".nv", &nv);
	
	int nt = msh->maxNumTriangles();
	if(!hasNamedAttr(".ntri"))
		addIntAttr(".ntri");
	
	writeIntAttr(".ntri", &nt);

	int nstg = msh->numStages();
	if(!hasNamedAttr(".nstg"))
		addIntAttr(".nstg");
	
	writeIntAttr(".nstg", &nstg);

	int nuvs = msh->numUVSets();
	if(!hasNamedAttr(".nuvs"))
		addIntAttr(".nuvs");
	
	writeIntAttr(".nuvs", &nuvs);
	
	writeF3Data(".pos", (char *)msh->c_positions(), nv);
	writeF3Data(".nml", (char *)msh->c_normals(), nv);
	writeI3Data(".ind", (char *)msh->c_indices(), nt);
	
	for(int i=0;i<nstg;++i) {
	    const std::string stgName = boost::str(boost::format("stg%1%") % i);
	    HCoarseFineHistory fhis(childPath(stgName));
	    fhis.save(&msh->stage(i));
	    fhis.close();
	}

	if(nuvs>0) {
	    HFaceVaryingUV fuv(childPath("face_varying_uv"));
	    fuv.save(msh, nt);
	    fuv.close();
	}

	return true;
}

bool HHistoryMesh::writeF3Data(const std::string &name,
				char *data, const int &count)
{
	typedef HOocArray<hdata::TFloat, 96, 96> F3Data;
	bool stat;
    F3Data *vec = createDataStorage<F3Data>(name, true, stat);
    if(!stat) {
    	std::cout << "\n ERROR cannot create ooc storage "<<name;
    	return false;
    }
	
    int nc = Round32(count)>>5;
    
    vec->writeColumns(data, 0, nc);
    delete vec;
	return true;
}

bool HHistoryMesh::writeI3Data(const std::string &name,
				char *data, const int &count)
{
	typedef HOocArray<hdata::TInt, 96, 96> I3Data;
	bool stat;
    I3Data *ind = createDataStorage<I3Data>(name, true, stat);
    if(!stat) {
    	std::cout << "\n ERROR cannot create ooc storage "<<name;
    	return false;
    }
	
    int nc = Round32(count)>>5;
    
    ind->writeColumns(data, 0, nc);
    delete ind;
	return true;
}

}
