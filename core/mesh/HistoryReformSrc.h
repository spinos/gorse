/*
 *  HistoryReformSrc.h
 *  aloe
 *
 *  select stage from source mesh which contains all stages
 *
 */

#ifndef ALO_HISTORY_REFORM_SRC_H
#define ALO_HISTORY_REFORM_SRC_H

#include "HistoryReform.h"

namespace alo {

class HistoryReformSrc : public HistoryReform {

public:
	HistoryReformSrc();

	void reformSrc(AdaptableMesh *outMesh, HistoryMesh *stageMesh, 
		const float &lod, const HistoryMesh *sourceMesh);

	void reformSrc1(AdaptableMesh *outMesh, HistoryMesh *stageMesh, 
		const int &lodNv, const HistoryMesh *sourceMesh);
	
protected:

private:

};

}

#endif
