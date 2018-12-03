/*
 *  Fissure.cpp
 *  aloe
 *
 * 
 */

#include "Fissure.h"
#include <geom/AdaptableMesh.h>
#include <bvh/BVH.h>
#include <bvh/BVHBuilder.h>
#include <bvh/BVHSplit.h>
#include <bvh/BVHPrimitive.h>
#include <sdb/L3Tree.h>

namespace alo {

Fissure::Fissure()
{ m_bvh = new BVH; }

Fissure::~Fissure()
{ delete m_bvh; }

void Fissure::granulate(const AdaptableMesh *inMesh)
{
	m_bvh->clear();

	BoundingBox box;
	BVHPrimitive ap;

	const int &nt = inMesh->numTriangles();
	for(int i=0;i<nt;++i) {

		inMesh->getTriangleAabb(box, i);

		ap.setAABB(box);
        ap.setIndex(i);

		m_bvh->addPrimitive(ap);
	}

	m_bvh->setRootLeaf();

	BVHSplit::InnerNumPrimitives = 1<<16;
	BVHSplit::LeafNumPrimitives = 1<<14;
	BVHBuilder builder;
    builder.build(m_bvh);

    std::cout<<" bvh "<<*m_bvh;
}

BVHNodeIterator Fissure::firstPart() const
{ return m_bvh->firstLeaf(); }

BVHNodeIterator Fissure::nextPart(BVHNodeIterator x) const
{ return m_bvh->nextLeaf(x); }

void Fissure::reformPart(AdaptableMesh *outMesh, BVHNodeIterator x, const AdaptableMesh *inMesh) const
{
	const BVHNode *leaf = x._node;
    int b = leaf->leafBegin();
	int e = leaf->leafEnd();
	sdb::L3Tree<int, int, 2048, 512, 1024> vertexMap;
	mapVertices(vertexMap, b, e, inMesh->c_indices());
	
	const int nv = vertexMap.size();
	const int nt = e - b;
	outMesh->createTriangleMesh(nv, nt);
	
	setMeshVertices(outMesh, vertexMap, b, e, inMesh);
	setMeshFaceIndices(outMesh->indices(), vertexMap, b, e, inMesh->c_indices() );
}

void Fissure::mapVertices(sdb::L3Tree<int, int, 2048, 512, 1024> &vertexMap, 
                        int pbegin, int pend, const Int3 *faces) const
{
    for(int i=pbegin;i<pend;++i) {
        const BVHPrimitive &p = m_bvh->c_primitives()[i];
        const Int3 &fv = faces[p.index()];
        vertexMap.insert(fv.x, 0);
        vertexMap.insert(fv.y, 0);
        vertexMap.insert(fv.z, 0);
    }
    
    int vcount = 0;
    sdb::L3Node<int, int, 1024> *block = vertexMap.begin();
    while(block) {
        for(int i=0;i<block->count();++i) {
            block->value(i) = vcount;
            vcount++;
        }
        block = vertexMap.next(block);  
    }
}

void Fissure::setMeshVertices(AdaptableMesh *outMesh,
                        sdb::L3Tree<int, int, 2048, 512, 1024> &vertexMap, 
                        int pbegin, int pend, const AdaptableMesh *inMesh) const
{
    Vector3F *pos = outMesh->positions();
    Vector3F *nml = outMesh->normals();
    
    const Vector3F *inpos = inMesh->c_positions();
    const Vector3F *innml = inMesh->c_normals();
    const Int3 *faces = inMesh->c_indices();
    
    for(int i=pbegin;i<pend;++i) {
        const BVHPrimitive &p = m_bvh->c_primitives()[i];
        const Int3 &fv = faces[p.index()];
        int *v0 = vertexMap.find(fv.x);
        int *v1 = vertexMap.find(fv.y);
        int *v2 = vertexMap.find(fv.z);
        
        pos[*v0] = inpos[fv.x];
        pos[*v1] = inpos[fv.y];
        pos[*v2] = inpos[fv.z];
        nml[*v0] = innml[fv.x];
        nml[*v1] = innml[fv.y];
        nml[*v2] = innml[fv.z];
    }
}

void Fissure::setMeshFaceIndices(Int3 *outFaces, sdb::L3Tree<int, int, 2048, 512, 1024> &vertexMap, 
                        int pbegin, int pend, const Int3 *faces) const
{
    for(int i=pbegin;i<pend;++i) {
        const BVHPrimitive &p = m_bvh->c_primitives()[i];
        const Int3 &fv = faces[p.index()];
        int *v0 = vertexMap.find(fv.x);
        int *v1 = vertexMap.find(fv.y);
        int *v2 = vertexMap.find(fv.z);
        outFaces[i - pbegin].set(*v0, *v1, *v2);
    }
}

}
