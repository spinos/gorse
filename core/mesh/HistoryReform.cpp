#include "HistoryReform.h"
#include "HistoryMesh.h"

namespace alo {

HistoryReform::HistoryReform() : m_selectedStage(-1)
{ 
	m_mesh = new HistoryMesh; 
	m_mesh->createTriangleMesh(1024, 1024);
	m_mesh->addHistoryStage();
}

HistoryReform::~HistoryReform()
{ delete m_mesh; }

void HistoryReform::reform(AdaptableMesh *outMesh, const float &lod, const HistoryMesh *sourceMesh)
{
	int istage;
	int selV;

	sourceMesh->selectStage(istage, selV, lod);
	
	const CoarseFineHistory &stage = m_mesh->stage(0);
	
	if(istage != m_selectedStage) {
		sourceMesh->copyStageTo(m_mesh, istage);
		sortCoarseFaces(0, stage.coarseMax(), stage.c_value() );
		//m_mesh->printHistoryStage(0); 
		m_selectedStage = istage;
	}

	m_mesh->copyTo(outMesh, selV, stage.fbegin());

	if(selV > stage.vbegin() || istage > 0) {

		int nc = searchCoarseBegin(selV, 0, stage.coarseMax(), stage.c_value() );
		//std::cout<<"\n coarse at " <<nc;

		int nf = searchFineEnd(selV, stage.fbegin(), stage.fineMax(), stage.c_value() );
		//std::cout<<" fine at " <<nf;

		int coarseL = stage.coarseMax() - nc;
		int fineL = nf - stage.fbegin();

		outMesh->removeLastFaces(coarseL);
/// append uv
		const int nuv = m_mesh->numUVSets();
		for(int i=0;i<nuv;++i) {
		    const Float2 *uvs = m_mesh->c_uvSet(i);
		    const Float2 *buv = &uvs[stage.fbegin() * 3];
		    outMesh->appendFaceUVs(buv, i, fineL);
		}
		
		const Int3 *b = &m_mesh->c_indices()[stage.fbegin()];
		outMesh->appendFaces(b, fineL);
	}

	//std::cout<< "\n  stage "<<istage
	//	<<" select nv "<<selV << " reformed nf "<<outMesh->numTriangles();
}

void HistoryReform::sortCoarseFaces(int first, int last, const int *history)
{
	if(last < first) return;
    int low, high;
    low = first;
    high = last;

    int list_separator = history[(first+last)/2];
    do
    {
        while(history[low] > list_separator) low++;
        while(history[high] < list_separator) high--;

        if(low<=high)
        {
        	m_mesh->swapHistory(low, high);
        	m_mesh->swapFace(low, high);
        	m_mesh->swapFaceUV(low, high);
            
            low++;
            high--;
        }
    } while(low<=high);
    
    if(first<high) sortCoarseFaces(first,high, history);
    if(low<last) sortCoarseFaces(low,last, history);
}

int HistoryReform::searchCoarseBegin(int nv, int first, int last, const int *history) const
{
	if(history[first] < nv) return first;
	if(history[last] > nv) return last;
	int low = first;
	int high = last;
	int mid = (first + last)>>1;
	while(high > low+1) {
		int vm = history[mid];
		if(vm > nv) low = mid;
		else high = mid;
		mid = (low + high)>>1;
	}
	
	return low;
}

int HistoryReform::searchFineEnd(int nv, int first, int last, const int *history) const
{
	if(history[first] > nv) return first;
	if(history[last] <= nv) return last+1;
	int low = first;
	int high = last;
	int mid = (first + last)>>1;
	while(high > low+1) {
		int vm = history[mid];
		if(vm <= nv) low = mid;
		else high = mid;
		mid = (low + high)>>1;
	}
	
	return high;
}

}
