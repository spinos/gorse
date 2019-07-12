/*
 *  HexahedronDistance.h
 *  gdf
 *
 *
 *  2019/7/12
 */

#ifndef ALO_HEXAHEDRON_DISTANCE_H
#define ALO_HEXAHEDRON_DISTANCE_H

#include <graph/BaseDistanceField.h>

namespace alo {

namespace gdf {

template<typename T>
class HexahedronDistance : public BaseDistanceField<T> {

	typedef BaseDistanceField<T> DFTyp;

public:

	HexahedronDistance();
	virtual ~HexahedronDistance();

/// connect nodes of a sparse cubic field Tf
	template<typename Tf, typename Tr>
	void buildGraph(Tf *field, Tr &rule);
/// geodesic distance from node closest to origin
	template<typename Tr>
	void computeDistance(Tr &rule);

};

template<typename T>
HexahedronDistance<T>::HexahedronDistance()
{}

template<typename T>
HexahedronDistance<T>::~HexahedronDistance()
{}

template<typename T>
template<typename Tf, typename Tr>
void HexahedronDistance<T>::buildGraph(Tf *field, Tr &rule)
{
	const int nv = field->numNodes();

	typedef sdb::L3Tree<sdb::Coord2, int, 2048, 512, 1024> EdgeMapTyp;
	typedef sdb::L3Node<sdb::Coord2, int, 1024> EdgeMapBlockTyp;

	EdgeMapTyp edgeMap;

	typename Tf::NodeIterTyp nodeIt = field->nodeBegin();
	for(int v1=0;!nodeIt.done();nodeIt.next(),++v1) {
		const int &ki = nodeIt.first;
		const int nconn = rule.numConnectionsPerNode();
		for(int j=0;j<nconn;++j) {
			int kj = rule.getNodeConnection(ki, j);

			if(kj == ki) continue;
			const int v2 = field->getNodeIndex(kj);
			if(v2 < 0) continue;

			sdb::Coord2 c = sdb::Coord2(v1, v2).ordered();
			edgeMap.insert(c, 0);
		}
  	}

  	const int ne = edgeMap.size();
  	std::cout << "\n n edge " << ne;
  	
	std::vector<int> edgeBegins;
	std::vector<int> edgeInds;

	DFTyp::MapVertexEdgeIndex<EdgeMapTyp, EdgeMapBlockTyp >(edgeBegins, edgeInds, edgeMap);

	int ni = edgeInds.size();
	std::cout << "\n n ind " << ni;
	DFTyp::create(nv, ne, ni);

	DistanceNode<T> *distNodes = DFTyp::nodes();
	nodeIt = field->nodeBegin();
	for(int v1=0;!nodeIt.done();nodeIt.next(),++v1) {
		float *p = (float *)&distNodes[v1].pos;
		rule.computePosition(p, nodeIt.first);
  	}

  	DFTyp::extractEdges(&edgeMap);
	DFTyp::extractEdgeBegins(edgeBegins);
	DFTyp::extractEdgeIndices(edgeInds);
    
	edgeBegins.clear();
	edgeInds.clear();
	
	DFTyp::calculateEdgeLength();

}

template<typename T>
template<typename Tr>
void HexahedronDistance<T>::computeDistance(Tr &rule)
{
	DFTyp::resetNodes(1e20f, sdf::StBackGround, sdf::StUnknown);
	DFTyp::resetEdges(sdf::StBackGround);

	const Vector3F &origin = rule.originPoint();

	int closestToOrigin = 0;
	float minD = 1e20f;
	const int &nv = numNodes();
	for(int i=0;i<nv;++i) {
		const DistanceNode<T> &ni = DFTyp::nodes()[i];
		float d = ni.pos.distanceTo(origin);
		if(minD > d) {
			minD = d;
			closestToOrigin = i;
		}
	}

	DFTyp::setNodeDistance(closestToOrigin, 0.f, false);
	DFTyp::fastMarchingMethod();

}

}

}

#endif
