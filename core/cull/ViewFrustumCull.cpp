/*
 *  ViewFrustumCull.cpp
 *  aloe
 *
 */

#include "ViewFrustumCull.h"
#include <bvh/BVH.h>
#include <bvh/BVHBuilder.h>
#include <bvh/BVHSplit.h>
#include <bvh/BVHPrimitive.h>
#include <math/AFrustum.h>
#include "ViewFrustumEvent.h"
#include "VisibilityState.h"
#include <deque>

namespace alo {

ViewFrustumCull::ViewFrustumCull()
{ m_bvh = new BVH; }

ViewFrustumCull::~ViewFrustumCull()
{ delete m_bvh; }

void ViewFrustumCull::create(const BVH *bvh)
{
	const int &n = bvh->numNodes();
	m_nodes.createBuffer(n);

	const BVHNode *ns = bvh->c_nodes();
	memcpy(m_nodes.data(), ns, n * sizeof(BVHNode) );

	m_nodeHexa.createBuffer(n);
	
	for(int i=0;i<n;++i)
		m_nodeHexa[i].set(ns[i].aabb());
	
	m_primInd.createBuffer(n);
	
	int primCount=0;
	for(int i=0;i<n;++i) {
		if(m_nodes[i].isLeaf()) {
			m_nodes[i].setLeaf(primCount, primCount+1);
			m_primInd[primCount] = primCount;
			primCount++;
		}
	}

/// primitive-to-node map
	SimpleBuffer<int> nodeInd;
	nodeInd.createBuffer(primCount);
	primCount=0;
	for(int i=0;i<n;++i) {
		if(m_nodes[i].isLeaf()) {
			nodeInd[primCount] = i;
			primCount++;
		}
	}

	m_primHexa.createBuffer(primCount);
	for(int i=0;i<primCount;++i)
		m_primHexa[i] = m_nodeHexa[nodeInd[i]];

}

float ViewFrustumCull::getMeanSize() const
{
#if 0
	float minD = 1e28f;
	float maxD = 1e-8f;
#endif
	float s = 0.f;
	const int &n = m_primHexa.count();
	for(int i=0;i<n;++i) {
		float di = m_primHexa[i].size();
		s += di;
#if 0
		if(minD > di) minD = di;
		if(maxD < di) maxD = di;
#endif
	}
#if 0
	std::cout << " min max size " << minD << " " << maxD;
#endif
	return s / (float)n;
}

void ViewFrustumCull::compare(VisibilityState *visibilities, const AFrustum &fru) const
{
	const AFrustum af = fru;
	ViewFrustumEvent *e = new ViewFrustumEvent(m_nodes[0], m_nodeHexa[0], af);
	std::deque<ViewFrustumEvent *> visitQueue;
	visitQueue.push_back(e);

	for(;;) {

		ViewFrustumEvent *active = visitQueue.front();

		ViewFrustumEvent::CompareResult res = active->result();
		if(active->hasChildren()) {
			int iLeft = active->leftChildInd();
			const BVHNode &lft = m_nodes[iLeft];
			const BVHNode &rgt = m_nodes[iLeft+1];

			ViewFrustumEvent *lftVisit = ViewFrustumEvent::Create(lft, m_nodeHexa[iLeft], af, res);
			visitQueue.push_back(lftVisit);

			ViewFrustumEvent *rgtVisit = ViewFrustumEvent::Create(rgt, m_nodeHexa[iLeft+1], af, res);
			visitQueue.push_back(rgtVisit);

		} else {
			const int le = active->leafEnd();
			for(int i=active->leafBegin();i<le;++i) {
				VisibilityState &vis = visibilities[m_primInd[i]];
				vis.setVisible(res < ViewFrustumEvent::rOutside);
			}
		}

		delete active;
		visitQueue.erase(visitQueue.begin());

		if(visitQueue.size() < 1) break;
	}

}

const Hexahedron &ViewFrustumCull::primitiveHexahedron(int i) const
{ return m_primHexa[i]; }

const int &ViewFrustumCull::numPrimitives() const
{ return m_primHexa.count(); }

void ViewFrustumCull::clearBvh()
{ m_bvh->clear(); }

void ViewFrustumCull::addBvhPrimitive(const BVHPrimitive &x)
{ m_bvh->addPrimitive(x); }

void ViewFrustumCull::buildBvh()
{
	m_bvh->setRootLeaf();

	BVHSplit::InnerNumPrimitives = 16;
	BVHSplit::LeafNumPrimitives = 4;

    BVHBuilder::Build(m_bvh);

    const int &n = m_bvh->numNodes();
	m_nodes.createBuffer(n);

	const BVHNode *ns = m_bvh->c_nodes();
	memcpy(m_nodes.data(), ns, n * sizeof(BVHNode) );

	m_nodeHexa.createBuffer(n);
	
	for(int i=0;i<n;++i)
		m_nodeHexa[i].set(ns[i].aabb());

	m_primInd.createBuffer(n);

	const int &primCount = m_bvh->numPrimitives();
	std::cout << " n primitive " << primCount;

/// primitive-to-node map
	SimpleBuffer<int> nodeInd;
	nodeInd.createBuffer(primCount);

	const BVHPrimitive *prims = m_bvh->c_primitives();

	for(int i=0;i<n;++i) {
		const BVHNode &ni = m_nodes[i];
		if(ni.isLeaf()) {

			int b = ni.leafBegin();
			int e = ni.leafEnd();
			
			for(int j=b;j<e;++j) {
				nodeInd[prims[j].index()] = j;
				m_primInd[j] = prims[j].index();
			}
			
		}
	}

	m_primHexa.createBuffer(primCount);
	for(int i=0;i<primCount;++i) {
		const BVHPrimitive &p = prims[nodeInd[i]];
		m_primHexa[i].set(p.aabb());
	}
}

}
