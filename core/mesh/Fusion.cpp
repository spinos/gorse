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

bool Fusion::combine(AdaptableMesh *outMesh, const std::vector<const AdaptableMesh *> &inMeshes)
{
    BoundingBox mergeBox;
    int nv = 0;
    int nt = 0;
    std::vector<const AdaptableMesh *>::const_iterator it = inMeshes.begin();
    for(;it!=inMeshes.end();++it) {
        const AdaptableMesh *inMesh = *it;
        
        nv += inMesh->numVertices();
        nt += inMesh->numTriangles();
        
        BoundingBox meshBox;
        inMesh->getAabb(meshBox);
        mergeBox.expandBy(meshBox);
    }
    
    m_mergeInds.resetBuffer(nv);
    m_mergeMap.resetBuffer(nv);
    
    const int mergedNv = mapMergeIndices(inMeshes, mergeBox);
    
    std::cout << "\n nv "<< nv << " merged nv " << mergedNv;
    outMesh->createTriangleMesh(mergedNv, nt);
    
    remapPositions(outMesh, nv);
    
    int voffset = 0;
    int toffset = 0;
    it = inMeshes.begin();
    for(;it!=inMeshes.end();++it) {  
        const AdaptableMesh *inMesh = *it;
        
        remapFaces(outMesh, toffset, inMesh, voffset);
        
        voffset += (*it)->numVertices();
        toffset += (*it)->numTriangles();
    }
    
    combineMeshUv(outMesh, inMeshes);
    
    return true;
}

void Fusion::combineMeshUv(AdaptableMesh *outMesh, const std::vector<const AdaptableMesh *> &inMeshes) const
{
    if(inMeshes[0]->numUVSets() < 1) return;
    
    const std::string &setName = inMeshes[0]->c_uvName(0);
    Float2 *ouv = outMesh->addUVSet(setName);
    
    int uvoffset = 0;
    std::vector<const AdaptableMesh *>::const_iterator it = inMeshes.begin();
    for(;it!=inMeshes.end();++it) {
        const AdaptableMesh *inMesh = *it;
        
        const Float2 *iuv = inMesh->c_uvSet(0);
        
        memcpy(&ouv[uvoffset], 
            iuv, inMesh->numTriangles() * 3 * 8);
            
        uvoffset += (*it)->numTriangles() * 3;
    }
}

int Fusion::mapMergeIndices(const std::vector<const AdaptableMesh *> &inMeshes,
                            const BoundingBox &bbox)
{
    sds::FZOrderCurve sfc;
    const Vector3F midP = bbox.center();
	const float spanL = bbox.getLongestDistance();
	sfc.setCoord(midP.x, midP.y, midP.z, spanL * .50045f);
    
    int totalNv = 0;
    std::vector<const AdaptableMesh *>::const_iterator it = inMeshes.begin();
    for(;it!=inMeshes.end();++it) {  
        const AdaptableMesh *inMesh = *it;
        
        const int &nv = inMesh->numVertices();
        for(int i=0;i<nv;++i) {
            MergeIndex &vi = m_mergeInds[totalNv];
            vi._pos = inMesh->c_positions()[i];
            vi._key = sfc.computeKey((const float *)&vi._pos);
            vi._ind = totalNv;
            vi._merged = -1;
            
            totalNv++;
        }
    }
    
    QuickSort1::SortByKey<int, MergeIndex>(m_mergeInds.data(), 0, totalNv - 1);
    
    int groupBegin = 0;
    int preKey = -1;
    for(int i=0;i<totalNv;++i) {
        MergeIndex &vi = m_mergeInds[i];
        const Vector3F &vipos = vi._pos;
        const int &ki = vi._key;
        if(preKey < ki) {
            preKey = ki;
            groupBegin = i;
        } else {
            for(int j=groupBegin;j<i;++j) {
                const MergeIndex &vj = m_mergeInds[j];
/// close enough merged with vj
                if((vipos - vj._pos).length2() < .8e-8f) {
                    vi._merged = vj._ind;
                    j = i;
                }
            }
        }
    }

    int uniqueNv = 0;
    for(int i=0;i<totalNv;++i) {
        const MergeIndex &vi = m_mergeInds[i];
        if(vi._merged < 0) { 
            m_mergeMap[vi._ind] = uniqueNv;
            uniqueNv++;
        } else {
            m_mergeMap[vi._ind] = m_mergeMap[vi._merged];
        }
    }
    
    return uniqueNv;
}

void Fusion::remapPositions(AdaptableMesh *outMesh, const int &nv) const
{
    Vector3F *pos = outMesh->positions();
    int uniqueNv = 0;
    for(int i=0;i<nv;++i) {
        const MergeIndex &vi = m_mergeInds[i];
        if(vi._merged < 0) {
            pos[uniqueNv] = vi._pos;
            uniqueNv++;
        }
    }
}

void Fusion::remapFaces(AdaptableMesh *outMesh, const int &faceOffset,
                    const AdaptableMesh *inMesh, const int &vertexOffset) const
{
    for(int i=0;i<inMesh->numTriangles();++i) {
        Int3 &fi = outMesh->indices()[i + faceOffset];
        const Int3 &src = inMesh->c_indices()[i];
        fi.x = m_mergeMap[src.x + vertexOffset];
        fi.y = m_mergeMap[src.y + vertexOffset];
        fi.z = m_mergeMap[src.z + vertexOffset];
    }
}
    
}
