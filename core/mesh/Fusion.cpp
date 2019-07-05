/*
 *  Fusion.cpp
 *  aloe
 *
 *  2019/7/6 
 */
 
#include "Fusion.h"
#include <geom/AdaptableMesh.h>
#include "MeshTopology.h"
#include <sds/FZOrder.h>
#include <math/QuickSort.h>

namespace alo {
    
Fusion::Fusion()
{}

Fusion::~Fusion()
{}

bool Fusion::combine(AdaptableMesh *outMesh, const std::vector<AdaptableMesh *> &inMeshes)
{
    BoundingBox mergeBox;
    int nv = 0;
    int nt = 0;
    std::vector<AdaptableMesh *>::const_iterator it = inMeshes.begin();
    for(;it!=inMeshes.end();++it) {
        AdaptableMesh *inMesh = *it;
        
        nv += inMesh->numVertices();
        nt += inMesh->numTriangles();
        
        BoundingBox meshBox;
        inMesh->getAabb(meshBox);
        mergeBox.expandBy(meshBox);
    }
    
    outMesh->createTriangleMesh(nv, nt);
    
    m_mergeInds.resetBuffer(nv);
    mapMergeIndices(inMeshes, mergeBox);
    
    int voffset = 0;
    int toffset = 0;
    it = inMeshes.begin();
    for(;it!=inMeshes.end();++it) {  
        AdaptableMesh *inMesh = *it;
        
        memcpy(&outMesh->positions()[voffset], 
            inMesh->c_positions(), inMesh->numVertices() * 12);
            
        memcpy(&outMesh->indices()[toffset], 
            inMesh->c_indices(), inMesh->numTriangles() * 12);
        
        voffset += (*it)->numVertices();
        toffset += (*it)->numTriangles();
    }
    
    combineMeshUv(outMesh, inMeshes);
    
    return true;
}

void Fusion::combineMeshUv(AdaptableMesh *outMesh, const std::vector<AdaptableMesh *> &inMeshes)
{
    if(inMeshes[0]->numUVSets() < 1) return;
    
    const std::string &setName = inMeshes[0]->c_uvName(0);
    Float2 *ouv = outMesh->addUVSet(setName);
    
    int uvoffset = 0;
    std::vector<AdaptableMesh *>::const_iterator it = inMeshes.begin();
    for(;it!=inMeshes.end();++it) {
        AdaptableMesh *inMesh = *it;
        
        const Float2 *iuv = inMesh->c_uvSet(0);
        
        memcpy(&ouv[uvoffset], 
            iuv, inMesh->numTriangles() * 3 * 8);
            
        uvoffset += (*it)->numTriangles() * 3;
    }
}

void Fusion::mapMergeIndices(const std::vector<AdaptableMesh *> &inMeshes,
                            const BoundingBox &bbox)
{
    sds::FZOrderCurve sfc;
    const Vector3F midP = bbox.center();
	const float spanL = bbox.getLongestDistance();
	sfc.setCoord(midP.x, midP.y, midP.z, spanL * .5005f);
    
    int acc = 0;
    std::vector<AdaptableMesh *>::const_iterator it = inMeshes.begin();
    for(;it!=inMeshes.end();++it) {  
        AdaptableMesh *inMesh = *it;
        
        const int &nv = inMesh->numVertices();
        for(int i=0;i<nv;++i) {
            MergeIndex &vi = m_mergeInds[acc];
            vi._key = sfc.computeKey((const float *)&inMesh->c_positions()[i]);
            vi._ind = acc;
            
            acc++;
        }
    }
    
    QuickSort1::SortByKey<int, MergeIndex>(m_mergeInds.data(), 0, acc - 1);
    
    int ng = 0;
    int preKey = -1;
    for(int i=0;i<acc;++i) {
        const int &ki = m_mergeInds[i]._key;
        if(preKey < ki) {
            preKey = ki;
            ng++;
        }
    }
    
    std::cout << "\n nv "<< acc << " ng " << ng;
}
    
}
