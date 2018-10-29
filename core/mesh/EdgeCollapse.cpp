#include "EdgeCollapse.h"
#include "EdgeIndex.h"
#include "EdgeValue.h"
#include "FaceIndex.h"
#include "FaceValue.h"
#include "VertexValue.h"
#include <geom/AdaptableMesh.h>

namespace alo {

EdgeCollapse::EdgeCollapse()
{}

EdgeCollapse::~EdgeCollapse()
{ 
	delete[] m_vertices; 
	m_edges.clear();
	m_tris.clear();
}

void EdgeCollapse::simplify(AdaptableMesh *msh)
{
	m_mesh = msh;
	
	buildTopology();

	computeVertexCost();
	computeEdgeCost();
	
	for(int i=0;i<2;++i) {
		EdgeIndex collapseEdgeI = findEdgeToCollapse();
		if(!collapseEdgeI.isValid()) break;

		int vert2Remove, vert2Keep;
		getVertexToRemove(vert2Remove, vert2Keep, collapseEdgeI);

		const int lastV = msh->numVertices() - 1;
		const bool toSwapVertex = lastV != vert2Remove;

		std::deque<FaceIndex> lastFaces;
		std::deque<FaceIndex> reducedFaces;
		std::vector<int> vaFaceInds;
		std::vector<int> vbFaceInds;
		
		const VertexValue &vrm = m_vertices[vert2Remove];
		vrm.getReformedFaces(reducedFaces, vert2Remove, lastV);

		std::cout<<"\n " << vrm <<"\n " << m_vertices[lastV];

		if(toSwapVertex) {
/// to be connected to relocated vertex
			getConnectedFaceInds(vaFaceInds, vrm);
			getConnectedFaceInds(vbFaceInds, m_vertices[lastV]);
			m_vertices[lastV].getReformedFaces(lastFaces, lastV, vert2Remove);
		}

/// add collapsed faces
		std::deque<FaceIndex> collapsedFaces;
		vrm.getCollapsedFaces(collapsedFaces, vert2Remove, vert2Keep);

/// remove faces connected to vertex 
		if(!removeFaces(vrm, vert2Remove)) break;
		m_vertices[vert2Remove].clearFaces();

		if(toSwapVertex) {
/// remove faces connected to last vertex
			if(!removeFaces(m_vertices[lastV], lastV)) break;
			m_vertices[lastV].clearFaces();
		}
	
		addFaces(collapsedFaces);
		addFaces(reducedFaces);

		if(toSwapVertex) {
			addFaces(lastFaces);
			relocateVertices(vert2Remove, lastV, vaFaceInds, vbFaceInds);
		}

		const int &ntri = msh->numTriangles();
/// to the very end
		relocateFacesTo(vaFaceInds, ntri - 1);

		const int nfRemove = reducedFaces.size();

/// to the end but before faces to be reduced
		insertFacesAt(collapsedFaces, ntri - nfRemove);

/// hide
		msh->removeLastVertices(1);
		m_mesh->removeLastFaces(nfRemove);

		//updateCost(collapsedFaces);
		// lockVertices(collapsedFaces);
		if(toSwapVertex)
			updateCost(lastFaces);
		
		std::cout<<"\n step " << i << " reduce to f "<<m_mesh->numTriangles()
				<<" v "<<m_mesh->numVertices();

		std::cout<<"\n va " << vert2Remove << m_vertices[vert2Remove];
		std::cout<<"\n vb " << lastV << m_vertices[lastV];
		if(!checkTopology() ) break;

	}

}

void EdgeCollapse::buildTopology()
{
	m_numFaces = m_mesh->numTriangles();
	m_numVertices = m_mesh->numVertices();
	m_vertices = new VertexValue[m_numVertices];

	const unsigned *ind = m_mesh->c_indices();
	for(int i=0;i<m_mesh->numIndices();i+=3) {
		m_edges[EdgeIndex(ind[i], ind[i + 1])] = EdgeValue();
		m_edges[EdgeIndex(ind[i + 1], ind[i + 2])] = EdgeValue();
		m_edges[EdgeIndex(ind[i + 2], ind[i])] = EdgeValue();
	}

	for(int i=0;i<m_mesh->numIndices();i+=3) {
		const int j = i/3;

		FaceIndex fi(ind[i], ind[i + 1], ind[i+2]);
		m_tris[fi] = FaceValue(j);

		m_edges[EdgeIndex(ind[i], ind[i + 1])].connectToFace(fi);
		m_edges[EdgeIndex(ind[i + 1], ind[i + 2])].connectToFace(fi);
		m_edges[EdgeIndex(ind[i + 2], ind[i])].connectToFace(fi);
		
		m_vertices[ind[i]].connectToFace(fi);
		m_vertices[ind[i + 1]].connectToFace(fi);
		m_vertices[ind[i + 2]].connectToFace(fi);
	}

	std::cout<<"\n n vert "<<m_numVertices
		<<" n edge "<<m_edges.size()
		<<" n face "<<m_tris.size();
}

void EdgeCollapse::computeVertexCost()
{
	const Vector3F *nmls = m_mesh->c_normals();
	for(int i=0;i<m_mesh->numVertices();++i) {
		if(!isVertexOnBorder(i, m_vertices[i]))
			m_vertices[i].computeCost(i, nmls);
	}
}

void EdgeCollapse::computeEdgeCost()
{
	std::map<EdgeIndex, EdgeValue>::iterator it = m_edges.begin();
	for(;it!=m_edges.end();++it) {
		if(!it->second.isOnBorder()) {
			it->second.cost() = computeEdgeCost(it->first);
		}
	}
}

EdgeIndex EdgeCollapse::findEdgeToCollapse()
{
	EdgeIndex collapseEdgeI;
	float edgeCostMin = 1e28f;
	int lastVert = m_mesh->numVertices() - 2;
	
	std::map<EdgeIndex, EdgeValue>::iterator it = m_edges.begin();
	for(;it!=m_edges.end();++it) {
		const EdgeIndex &ei = it->first;
		const VertexValue &va = m_vertices[ei.v0()];
		const VertexValue &vb = m_vertices[ei.v1()];
		if(va.lastConnectedVertex() > lastVert 
			|| vb.lastConnectedVertex() > lastVert)
			continue;

		if(va.connectedFaces().size() < 3 
			|| vb.connectedFaces().size() < 3 )
			continue;

		if(isVertexOnBorder(ei.v0(), va)
			&& isVertexOnBorder(ei.v1(), vb))
			continue;

		if(edgeCostMin > it->second.cost()) {
			edgeCostMin = it->second.cost();
			collapseEdgeI = it->first;
		}
	}
	std::cout<<"\n collapse edge "<<collapseEdgeI
		<<" cost "<<edgeCostMin;
		
	return collapseEdgeI;
}

void EdgeCollapse::getVertexToRemove(int &a, int &b, const EdgeIndex &ei)
{
	a = ei.v0();
	b = ei.v1();
	int c;
/// lower cost
	if(m_vertices[a].cost() > m_vertices[b].cost()) {
		c = a;
		a = b;
		b = c;
	}
/// cannot on border
	if(isVertexOnBorder(a, m_vertices[a])) {
		c = a;
		a = b;
		b = c;
	}

	std::cout<<" remove vert "<<a;
}

float EdgeCollapse::computeEdgeCost(const EdgeIndex &ei) const
{
	const Vector3F *pos = m_mesh->c_positions();
	float c = m_vertices[ei.v0()].cost();
	float cb = m_vertices[ei.v1()].cost();
	if(c < cb) c = cb;

	return pos[ei.v0()].distanceTo(pos[ei.v1()]) * c;
}

bool EdgeCollapse::isVertexOnBorder(int vi, const VertexValue &vert)
{ 
	std::deque<FaceIndex>::const_iterator it = vert.connectedFaces().begin();
	for(;it!=vert.connectedFaces().end();++it) {
		const FaceIndex &fi = *it;

		if(fi.v0() == vi || fi.v1() == vi) {
			EdgeIndex e0(fi.v0(), fi.v1());
			if(m_edges[e0].isOnBorder()) 
				return true;
		}

		if(fi.v1() == vi || fi.v2() == vi) {
			EdgeIndex e1(fi.v1(), fi.v2());
			if(m_edges[e1].isOnBorder()) 
				return true;
		}

		if(fi.v2() == vi || fi.v0() == vi) {
			EdgeIndex e2(fi.v2(), fi.v0());
			if(m_edges[e2].isOnBorder()) 
				return true;
		}
	}
	return false; 
}

bool EdgeCollapse::removeFaces(const VertexValue &vert, int vi)
{
	std::deque<int> neivs;
	vert.getConnectedVertices(neivs, vi);
	std::deque<int>::const_iterator it = neivs.begin();
	for(;it!=neivs.end();++it) {
		std::map<EdgeIndex, EdgeValue>::iterator fe = m_edges.find(EdgeIndex(vi, *it));
		if(fe == m_edges.end()) {
			std::cout<< "\n\n ERROR cannot remove nonexistent edge "<<EdgeIndex(vi, *it);
			return false;
		}
			
		m_edges.erase(fe);

	}

	std::deque<FaceIndex>::const_iterator fit = vert.connectedFaces().begin();
	for(;fit!= vert.connectedFaces().end();++fit) {
		const FaceIndex &fi = *fit;

		std::map<FaceIndex, FaceValue>::iterator ff = m_tris.find(fi);
		if(ff == m_tris.end()) {
			std::cout<< "\n\n ERROR cannot remove nonexistent face "<<fi;
			return false;
		}

		std::cout<<"\n remove face "<<fi;
		m_tris.erase(ff);

		if(fi.v0() != vi)
			m_vertices[fi.v0()].disconnectFace(fi);

		if(fi.v1() != vi)
			m_vertices[fi.v1()].disconnectFace(fi);

		if(fi.v2() != vi)
			m_vertices[fi.v2()].disconnectFace(fi);
	}
	
	return true;
}

void EdgeCollapse::removeEdge(const EdgeIndex &ei)
{
	std::map<EdgeIndex, EdgeValue>::iterator fe = m_edges.find(ei);
	if(fe != m_edges.end())
		m_edges.erase(fe);
}

bool EdgeCollapse::removeFace(const FaceIndex &fi)
{
	std::map<FaceIndex, FaceValue>::iterator ff = m_tris.find(fi);
	if(ff == m_tris.end()) {
		std::cout<< "\n\n ERROR cannot remove nonexistent face "<<fi;
		return false;
	}
	
	m_tris.erase(ff);

	m_vertices[fi.v0()].disconnectFace(fi);
	m_vertices[fi.v1()].disconnectFace(fi);
	m_vertices[fi.v2()].disconnectFace(fi);

	return true;
}

void EdgeCollapse::addFaces(const std::deque<FaceIndex> &faces)
{
	std::deque<FaceIndex>::const_iterator it = faces.begin();
	for(;it!=faces.end();++it) {

		const FaceIndex &fi = *it;
		std::cout<<"\n add face "<<fi;
		
		EdgeIndex e0(fi.v0(), fi.v1());
		addEdge(e0);
		m_edges[e0].connectToFace(fi);

		EdgeIndex e1(fi.v1(), fi.v2());
		addEdge(e1);
		m_edges[e1].connectToFace(fi);

		EdgeIndex e2(fi.v2(), fi.v0());
		addEdge(e2);
		m_edges[e2].connectToFace(fi);

		m_tris[fi] = FaceValue();

		m_vertices[fi.v0()].connectToFace(fi);
		m_vertices[fi.v1()].connectToFace(fi);
		m_vertices[fi.v2()].connectToFace(fi);
	}
}

void EdgeCollapse::addEdge(const EdgeIndex &e)
{
	if(m_edges.find(e) == m_edges.end()) {
		m_edges[e] = EdgeValue();
		//std::cout << " add edge " << e;
	}
}

void EdgeCollapse::getConnectedFaceInds(std::vector<int> &faceInds,
                const VertexValue &vert)
{
	std::deque<FaceIndex>::const_iterator it = vert.connectedFaces().begin();
	for(;it!=vert.connectedFaces().end();++it) {
		const FaceIndex &fi = *it;

		if(m_tris.find(fi) == m_tris.end()) {
			std::cout << "\n\n ERROR nonexistent face " << fi;
			return;
		}

		faceInds.push_back(m_tris[fi].ind());
	}
}

void EdgeCollapse::relocateVertices(int va, int vb,
                const std::vector<int> &vaFaces,
                const std::vector<int> &vbFaces)
{
	std::cout<<"\n swap v " << va << " to "<<vb;
	m_mesh->swapVertex(va, vb,
				vaFaces, vbFaces);
	std::vector<int>::const_iterator it = vaFaces.begin();
	for(;it!=vaFaces.end();++it)
		setFaceInd(*it);

	it = vbFaces.begin();
	for(;it!=vbFaces.end();++it)
		setFaceInd(*it);
}

void EdgeCollapse::setFaceInd(int i)
{
	const unsigned *inds = m_mesh->c_indices();
	int fv[3];
	fv[0] = inds[i * 3];
	fv[1] = inds[i * 3 + 1];
	fv[2] = inds[i * 3 + 2];
	FaceIndex fi(fv[0], fv[1], fv[2]);
	if(m_tris.find(fi) == m_tris.end()) {
		std::cout << "\n\n ERROR nonexistent face " << fi;
		return;
	}
	m_tris[fi].ind() = i;
}

void EdgeCollapse::updateCost(const std::deque<FaceIndex> &faces)
{
	const unsigned *ind = m_mesh->c_indices();
	const Vector3F *nmls = m_mesh->c_normals();
	std::deque<FaceIndex>::const_iterator it = faces.begin();
	for(;it!=faces.end();++it) {

		const FaceIndex &fi = *it;

		m_vertices[fi.v0()].computeCost(fi.v0(), nmls);
		m_vertices[fi.v1()].computeCost(fi.v1(), nmls);
		m_vertices[fi.v2()].computeCost(fi.v2(), nmls);
	}

	it = faces.begin();
	for(;it!=faces.end();++it) {
		const FaceIndex &fi = *it;
		const EdgeIndex e0(fi.v0(), fi.v1());
		if(!m_edges[e0].isOnBorder()) {
			m_edges[e0].cost() = computeEdgeCost(e0);
		}
		const EdgeIndex e1(fi.v1(), fi.v2());
		if(!m_edges[e1].isOnBorder()) {
			m_edges[e1].cost() = computeEdgeCost(e1);
		}
		const EdgeIndex e2(fi.v2(), fi.v0());
		if(!m_edges[e2].isOnBorder()) {
			m_edges[e2].cost() = computeEdgeCost(e2);
		}
	}
}

void EdgeCollapse::relocateFacesTo(const std::vector<int> &faces, int toLastFace)
{
	int toFace = toLastFace;
	std::vector<int>::const_iterator it = faces.begin();

    for(;it!=faces.end();++it) {
    	std::cout<<"\n swap f " << *it << "  "<<toFace;

    	const unsigned *afv = &m_mesh->c_indices()[*it * 3];
    	int fv[3];
    	fv[0] = afv[0];
    	fv[1] = afv[1];
    	fv[2] = afv[2];
    	FaceIndex afi(fv[0], fv[1], fv[2]);

    	if(m_tris.find(afi) == m_tris.end()) {
    		std::cout << "\n\n ERROR nonexistent face "<<afi;
    	} else {
    		m_tris[afi].ind() = toFace;
    	}
   	
    	const unsigned *bfv = &m_mesh->c_indices()[toFace * 3];

    	fv[0] = bfv[0];
    	fv[1] = bfv[1];
    	fv[2] = bfv[2];
    	FaceIndex bfi(fv[0], fv[1], fv[2]);

    	if(m_tris.find(bfi) == m_tris.end()) {
    		std::cout << "\n\n ERROR nonexistent face "<<bfi;
    	} else {
    		m_tris[bfi].ind() = *it;
    	}

    	m_mesh->swapFace(*it, toFace);

/// move forward
    	toFace--;
    }
}

void EdgeCollapse::insertFacesAt(const std::deque<FaceIndex> &faces, 
                        int location)
{
	const Vector3F *pos = m_mesh->c_positions();
	const Vector3F *nml = m_mesh->c_normals();
	std::vector<int> cfv;
	std::deque<FaceIndex>::const_iterator it = faces.begin();
	for(;it!=faces.end();++it) {
		const FaceIndex &fi = *it;
		const Vector3F &p0 = pos[fi.v0()];
		const Vector3F &p1 = pos[fi.v1()];
		const Vector3F &p2 = pos[fi.v2()];
		Vector3F nf = nml[fi.v0()] + nml[fi.v1()] + nml[fi.v2()];
		Vector3F n = (p1 - p0).cross(p2 - p0);

		cfv.push_back(fi.v0());
		if(n.dot(nf) > 0) {
			cfv.push_back(fi.v1());
			cfv.push_back(fi.v2());
		} else {
			cfv.push_back(fi.v2());
			cfv.push_back(fi.v1());
		}

		std::cout<<" insert " << fi;
	}

	m_numFaces += faces.size(); std::cout << "\n history length "<< m_numFaces;

	m_mesh->insertFaces(cfv, location);

	for(int i=location;i<m_numFaces;++i) {
		const unsigned *fv = &m_mesh->c_indices()[i * 3];
		const FaceIndex fi(fv[0], fv[1], fv[2]);

		if(m_tris.find(fi) == m_tris.end() ) {
			std::cout << "\n\n ERROR nonexistent face" <<fi;
		} else {
			m_tris[fi].ind() = i;
			//std::cout<< "\n f "<<i<<fi;
		}
	}
}

bool EdgeCollapse::checkTopology()
{
	const unsigned *inds = m_mesh->c_indices();
	for(int i=0;i<m_numVertices;++i) {
		if(!m_vertices[i].check(i)) return false;
		std::deque<FaceIndex>::const_iterator it = m_vertices[i].connectedFaces().begin();
		for(;it!=m_vertices[i].connectedFaces().end();++it) {
			const FaceIndex &fi = *it;
			if(m_tris.find(fi) == m_tris.end()) {
				std::cout << "\n\n ERROR v "<<i<<" nonexistent face " << fi;
				return false;
			}
			int find = m_tris[fi].ind();
			if(find < 0 || find > m_mesh->numTriangles() * 2) {
				std::cout << "\n\n ERROR v "<<i<<" invalid face ind " << find;
				return false;
			}

			int fv[3];
			fv[0] = inds[find * 3];
			fv[1] = inds[find * 3 + 1];
			fv[2] = inds[find * 3 + 2];

			if(!fi.hasV(fv[0]) 
				|| !fi.hasV(fv[1])
				|| !fi.hasV(fv[2])) {

				std::cout << "\n\n ERROR v "<<i<<" mismatch face " << fi
					<< " found "<<find<<" (" << fv[0] << " " << fv[1] << " " << fv[2] << ") ";
				return false;
			}
		}
	}
	std::cout<< "\n passed checkTopology ";
	return true;
}

}
