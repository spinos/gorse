/*
 *  ViewFrustumCull.cpp
 *  aloe
 *
 */

#include "ViewFrustumCull.h"
#include <bvh/BVH.h>
#include <math/AFrustum.h>
#include "ViewFrustumEvent.h"
#include "VisibilityState.h"
#include <deque>

namespace alo {

ViewFrustumCull::ViewFrustumCull()
{}

ViewFrustumCull::~ViewFrustumCull()
{}

void ViewFrustumCull::create(const BVH *bvh)
{
	const int &n = bvh->numNodes();
	m_nodes.createBuffer(n);

	const BVHNode *ns = bvh->c_nodes();
	memcpy(m_nodes.data(), ns, n * sizeof(BVHNode) );

	m_leafInds.createBuffer(n);

	int leafCount=0;
	for(int i=0;i<n;++i) {
		if(m_nodes[i].isLeaf()) {
			m_nodes[i].setLeaf(leafCount, leafCount+1);
			m_leafInds[leafCount] = i;
			leafCount++;
		}
	}
	
	m_hexa.createBuffer(n);
	
	for(int i=0;i<n;++i)
		m_hexa[i].set(ns[i].aabb());

}

void ViewFrustumCull::compare(VisibilityState *visibilities, const AFrustum &fru) const
{
	ViewFrustumEvent *e = new ViewFrustumEvent(m_nodes[0], m_hexa[0], fru);
	std::deque<ViewFrustumEvent *> visitQueue;
	visitQueue.push_back(e);

	for(;;) {

		ViewFrustumEvent *active = visitQueue.front();

		int iLeft = active->leftChildInd();
		ViewFrustumEvent::CompareResult res = active->result();
		if(iLeft > -1) {
			const BVHNode &lft = m_nodes[iLeft];
			const BVHNode &rgt = m_nodes[iLeft+1];

			ViewFrustumEvent *lftVisit = ViewFrustumEvent::Create(lft, m_hexa[iLeft], fru, res);
			visitQueue.push_back(lftVisit);

			ViewFrustumEvent *rgtVisit = ViewFrustumEvent::Create(rgt, m_hexa[iLeft+1], fru, res);
			visitQueue.push_back(rgtVisit);

		} else {
			VisibilityState &vis = visibilities[active->leafInd()];
			vis.setVisible(res < ViewFrustumEvent::rOutside);
		}

		delete active;
		visitQueue.erase(visitQueue.begin());

		if(visitQueue.size() < 1) break;
	}

}

const Hexahedron *ViewFrustumCull::c_hexahedrons() const
{ return m_hexa.c_data(); }

const Hexahedron &ViewFrustumCull::leafHexahedron(int i) const
{ return m_hexa[m_leafInds[i]]; }

}
