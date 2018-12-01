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
	std::cout<<"\n face ["<<b<<":"<<e<<"]";

}


}
