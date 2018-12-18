#include "HistoryReformSrc.h"
#include "HistoryMesh.h"

namespace alo {

HistoryReformSrc::HistoryReformSrc()
{}

bool HistoryReformSrc::reformF(AdaptableMesh *outMesh, HistoryMesh *stageMesh, 
					const float &lod, const HistoryMesh *sourceMesh)
{
	int istage;
	int selV;
	sourceMesh->selectStage(istage, selV, lod);
    if(selV == outMesh->numVertices()) return false;

	reformC(outMesh, stageMesh, istage, selV, sourceMesh);
    return true;
}

bool HistoryReformSrc::reformI(AdaptableMesh *outMesh, HistoryMesh *stageMesh, 
					const int &lodNv, const HistoryMesh *sourceMesh)
{
	int istage;
	int selV;
	sourceMesh->selectStageByVertexCount(istage, selV, lodNv);
	if(selV == outMesh->numVertices()) return false;
    
	reformC(outMesh, stageMesh, istage, selV, sourceMesh);
    return true;
}

void HistoryReformSrc::reformC(AdaptableMesh *outMesh, HistoryMesh *stageMesh, 
					const int &selStage, const int &selV, const HistoryMesh *sourceMesh)
{
	const CoarseFineHistory &stage = stageMesh->stage(0);
	
	if(selStage != stageMesh->cachedStageId() ) {
		sourceMesh->copyStageTo(stageMesh, selStage);
		sortCoarseFaces(stageMesh, 0, stage.coarseMax(), stage.c_value() );
	}

	reform(outMesh, stageMesh, selV, selStage, stage);
}

}
