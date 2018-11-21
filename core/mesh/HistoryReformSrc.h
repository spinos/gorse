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

	int m_selectedStage;

public:
	HistoryReformSrc();
	~HistoryReformSrc();

	void reformSrc(AdaptableMesh *outMesh, HistoryMesh *stageMesh, 
		const float &lod, const HistoryMesh *sourceMesh);
	
protected:

private:

};

}

#endif
