#include "HFaceVaryingUV.h"
#include <geom/V1ATriangleMesh.h>
#include <h5/HOocArray.h>
#include <math/roundi.h>
#include <h5/PathList.h>

namespace alo {

HFaceVaryingUV::HFaceVaryingUV(const std::string & path) : ver1::HBase(path) 
{}

HFaceVaryingUV::~HFaceVaryingUV()
{}

bool HFaceVaryingUV::save(const ver1::ATriangleMesh *msh, const int &ntri)
{ 
    int nuvs = msh->numUVSets();
    for(int i=0;i<nuvs;++i)
		writeUVData(msh->c_uvName(i), (char *)msh->c_uvSet(i), ntri);
	
    return true; 
}

bool HFaceVaryingUV::writeUVData(const std::string &name, 
                        char *data, const int &ntri)
{
/// 32 uvs 64 floats per column
    typedef HOocArray<hdata::TFloat, 64, 64> FUVData;
	bool stat;
    FUVData *uvd = createDataStorage<FUVData>(name, true, stat);
    if(!stat) {
    	std::cout << "\n ERROR cannot create ooc storage "<<name;
    	return false;
    }
    
    int count = ntri * 3;
    int nc = Round32(count)>>5;
    
    uvd->writeColumns(data, 0, nc);
    delete uvd;
    return true;
}

bool HFaceVaryingUV::load(ver1::ATriangleMesh *msh, const int &ntri)
{ 
    std::vector<std::string> uvNames;
    lsData(uvNames);
    std::vector<std::string>::const_iterator it = uvNames.begin();
    for(;it!=uvNames.end();++it) {
        PathList pl(*it);
        const std::string &uvs = pl.lastName();
        Float2 *uvd = msh->addUVSet(uvs);
        readUVData(uvs, (char *)uvd, ntri);
    }
        
    return true; 
}

bool HFaceVaryingUV::readUVData(const std::string &name, 
                        char *data, const int &ntri)
{
/// 32 uvs 64 floats per column
    typedef HOocArray<hdata::TFloat, 64, 64> FUVData;
	bool stat;
    FUVData *uvd = openDataStorage<FUVData>(name, stat);
    if(!stat) {
    	std::cout << "\n ERROR cannot open ooc storage "<<name;
    	return false;
    }
    
    int count = ntri * 3;
    int nc = Round32(count)>>5;
    
    uvd->readColumns(data, 0, nc);
    delete uvd;
    return true;
}

}
