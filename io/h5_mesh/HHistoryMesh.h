/*
 *  HHistoryMesh.h
 *  aloe
 *
 *  iattr: nv ntri nstg nuvs
 *  f3data: pos nml
 *  i3data: ind
 *  coarse_fine_history /stg[0:nstg-1]
 *  /face_varying_uv
 *
 */
#ifndef ALO_H5_HISTORY_MESH_H
#define ALO_H5_HISTORY_MESH_H

#include <h5/V1HBase.h>

namespace alo {

class HistoryMesh;

class HHistoryMesh : public ver1::HBase {
public:
	HHistoryMesh(const std::string & path);
	virtual ~HHistoryMesh();
	
	virtual bool verifyType() override;
	virtual bool save(const HistoryMesh *msh);
	virtual bool load(HistoryMesh *msh);
	
protected:

private:
	bool writeF3Data(const std::string &name,
				char *data, const int &count);
	bool writeI3Data(const std::string &name,
				char *data, const int &count);
};

}

#endif
