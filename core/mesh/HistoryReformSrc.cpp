#include "HistoryReformSrc.h"
#include "HistoryMesh.h"

namespace alo {

HistoryReformSrc::HistoryReformSrc()
{}

bool HistoryReformSrc::reformSrc(AdaptableMesh *outMesh, HistoryMesh *stageMesh, 
					const float &lod, const HistoryMesh *sourceMesh)
{
	int istage;
	int selV;
	sourceMesh->selectStage(istage, selV, lod);
    if(selV == stageMesh->cachedNv()) return false;
	
	const CoarseFineHistory &stage = stageMesh->stage(0);
	
	if(istage != stageMesh->cachedStageId() ) {
		sourceMesh->copyStageTo(stageMesh, istage);
		sortCoarseFaces(stageMesh, 0, stage.coarseMax(), stage.c_value() );
	}

	reform(outMesh, stageMesh, selV, istage, stage);
    return true;
}

bool HistoryReformSrc::reformSrc1(AdaptableMesh *outMesh, HistoryMesh *stageMesh, 
					const int &lodNv, const HistoryMesh *sourceMesh)
{
	int istage;
	int selV;
	sourceMesh->selectStageByVertexCount(istage, selV, lodNv);
	if(selV == stageMesh->cachedNv()) return false;
    
	const CoarseFineHistory &stage = stageMesh->stage(0);
	
	if(istage != stageMesh->cachedStageId() ) {
		sourceMesh->copyStageTo(stageMesh, istage);
		sortCoarseFaces(stageMesh, 0, stage.coarseMax(), stage.c_value() );
	}

	reform(outMesh, stageMesh, selV, istage, stage);
    return true;
}

}
