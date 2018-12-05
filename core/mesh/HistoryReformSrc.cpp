#include "HistoryReformSrc.h"
#include "HistoryMesh.h"

namespace alo {

HistoryReformSrc::HistoryReformSrc() : m_selectedStage(-1)
{}

HistoryReformSrc::~HistoryReformSrc()
{}

void HistoryReformSrc::reformSrc(AdaptableMesh *outMesh, HistoryMesh *stageMesh, 
					const float &lod, const HistoryMesh *sourceMesh)
{
	int istage;
	int selV;
	sourceMesh->selectStage(istage, selV, lod);
	
	const CoarseFineHistory &stage = stageMesh->stage(0);
	
	if(istage != m_selectedStage) {
		sourceMesh->copyStageTo(stageMesh, istage);
		sortCoarseFaces(stageMesh, 0, stage.coarseMax(), stage.c_value() );
		//stageMesh->printHistoryStage(0); 
		m_selectedStage = istage;
	}

	reform(outMesh, stageMesh, selV, istage, stage);
}

}
