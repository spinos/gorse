/*
 *  HFaceVaryingUV.h
 *  aloe
 *
 *  each uv set as a named data of float2
 *  length num tri * 3
 *
 */

#ifndef ALO_H_FACE_VARYING_UV_H
#define ALO_H_FACE_VARYING_UV_H

#include <h5/V1HBase.h>

namespace alo {

namespace ver1 {
class ATriangleMesh;
}

class HFaceVaryingUV : public ver1::HBase {

public:
    HFaceVaryingUV(const std::string & path);
    virtual ~HFaceVaryingUV();
    
    virtual bool save(const ver1::ATriangleMesh *msh, const int &ntri);
	virtual bool load(ver1::ATriangleMesh *msh, const int &ntri);

private:
    bool writeUVData(const std::string &name, 
                        char *data, const int &ntri);
	
};

}

#endif

