#include "EdgeCollapse.h"
#include "EdgeIndex.h"
#include "EdgeValue.h"
#include "VertexValue.h"
#include <geom/AdaptableMesh.h>

namespace alo {

EdgeCollapse::EdgeCollapse()
{}

void EdgeCollapse::simplify(AdaptableMesh *msh)
{
	m_mesh = msh;

	std::vector<VertexValue> vertices;
	std::map<EdgeIndex, EdgeValue> edges;
	buildTopology(vertices, edges);

	computeVertexCost(vertices, edges);
	computeEdgeCost(edges, vertices);
	
	for(int i=0;i<17;++i) {
		EdgeIndex collapseEdgeI = findEdgeToCollapse(vertices, edges);

	int vert2Remove, vert2Keep;
	getVertexToRemove(vert2Remove, vert2Keep, collapseEdgeI, vertices);

	const VertexValue &vrm = vertices[vert2Remove];
	removeConnectedEdges(edges, vrm, vert2Remove);
	disconnectedFaces(vertices, vrm, vert2Remove);

	std::vector<int> cfv;
	getCollapsedFaces(cfv,
			vrm.faceIndices(),
			vert2Remove, vert2Keep);

	addFaceEdges(edges, cfv);

	const int &ntri = msh->numTriangles();
	const int nfRemove = vrm.numFaces();
	connectFaceToVertices(vertices, cfv, ntri - nfRemove);

	const int lastV = msh->numVertices() - 1;
	const VertexValue lastVert = vertices[lastV];
	
		std::cout<<"\n swap v " << vert2Remove << " to "<<lastV;

		msh->swapVertex(vert2Remove, lastV,
				vrm.faceIndices(),
				vertices[lastV].faceIndices() );


		relocateFacesTo(vrm.faceIndices(), ntri - 1, vertices);
		//vertices[vert2Remove] = lastVert;
		
		msh->insertFaces(cfv, ntri - nfRemove);

	msh->removeLastVertices(1);
	msh->removeLastFaces(nfRemove);
		
		updateCost(vertices, cfv);

	}
}

void EdgeCollapse::buildTopology(std::vector<VertexValue> &vertices,
                    std::map<EdgeIndex, EdgeValue> &edges)
{
	for(int i=0;i<m_mesh->numVertices();++i)
		vertices.push_back(VertexValue());

	const unsigned *ind = m_mesh->c_indices();
	for(int i=0;i<m_mesh->numIndices();i+=3) {
		edges[EdgeIndex(ind[i], ind[i + 1])] = EdgeValue();
		edges[EdgeIndex(ind[i + 1], ind[i + 2])] = EdgeValue();
		edges[EdgeIndex(ind[i + 2], ind[i])] = EdgeValue();
	}

	for(int i=0;i<m_mesh->numIndices();i+=3) {
		const int j = i/3;
		edges[EdgeIndex(ind[i], ind[i + 1])].connectToFace(j);
		edges[EdgeIndex(ind[i + 1], ind[i + 2])].connectToFace(j);
		edges[EdgeIndex(ind[i + 2], ind[i])].connectToFace(j);
		vertices[ind[i]].connectToFace(j);
		vertices[ind[i + 1]].connectToFace(j);
		vertices[ind[i + 2]].connectToFace(j);
	}

	std::cout<<"\n n vertices "<<vertices.size()
		<<" n edges "<<edges.size();
}

void EdgeCollapse::computeVertexCost(std::vector<VertexValue> &vertices,
                    std::map<EdgeIndex, EdgeValue> &edges)
{
	const unsigned *ind = m_mesh->c_indices();
	const Vector3F *nmls = m_mesh->c_normals();
	for(int i=0;i<m_mesh->numVertices();++i) {
		if(!isVertexOnBorder(i, vertices[i], edges))
			vertices[i].computeCost(i, ind, nmls);
	}
}

void EdgeCollapse::computeEdgeCost(std::map<EdgeIndex, EdgeValue> &edges,
                    const std::vector<VertexValue> &vertices)
{
	std::map<EdgeIndex, EdgeValue>::iterator it = edges.begin();
	for(;it!=edges.end();++it) {
		if(!it->second.isOnBorder()) {
			it->second.cost() = computeEdgeCost(it->first, vertices);
		}
	}
}

EdgeIndex EdgeCollapse::findEdgeToCollapse(std::vector<VertexValue> &vertices,
                    std::map<EdgeIndex, EdgeValue> &edges)
{
	EdgeIndex collapseEdgeI;
	float edgeCostMin = 1e28f;
	
	std::map<EdgeIndex, EdgeValue>::iterator it = edges.begin();
	for(;it!=edges.end();++it) {
		if(!it->second.isOnBorder()) {
			if(edgeCostMin > it->second.cost()) {
				edgeCostMin = it->second.cost();
				collapseEdgeI = it->first;
			}
		}
	}
	std::cout<<"\n collapse edge "<<collapseEdgeI
		<<" cost "<<edgeCostMin;
		
	return collapseEdgeI;
}

void EdgeCollapse::getVertexToRemove(int &a, int &b,
				const EdgeIndex &ei, const std::vector<VertexValue> &vertices)
{
	a = ei.v0();
	b = ei.v1();
	if(vertices[a].cost() > vertices[b].cost()) {
		a = ei.v1();
		b = ei.v0();
	}

	std::cout<<" remove vertex "<<a;
}

float EdgeCollapse::computeEdgeCost(const EdgeIndex &ei,
			const std::vector<VertexValue> &vert) const
{
	const Vector3F *pos = m_mesh->c_positions();
	float c = vert[ei.v0()].cost();
	float cb = vert[ei.v1()].cost();
	if(c < cb) c = cb;

	return pos[ei.v0()].distanceTo(pos[ei.v1()]) * c;
}

bool EdgeCollapse::isVertexOnBorder(int vi, const VertexValue &vert,
			std::map<EdgeIndex, EdgeValue> &edges)
{ 
	const unsigned *indices = m_mesh->c_indices();
	std::vector<int>::const_iterator it = vert.faceIndices().begin();
	for(;it!=vert.faceIndices().end();++it) {
		const unsigned *fi = &indices[*it * 3];

		if(fi[0] == vi || fi[1] == vi) {
			EdgeIndex e0(fi[0], fi[1]);
			if(edges[e0].isOnBorder()) 
				return true;
		}

		if(fi[1] == vi || fi[2] == vi) {
			EdgeIndex e1(fi[1], fi[2]);
			if(edges[e1].isOnBorder()) 
				return true;
		}

		if(fi[2] == vi || fi[0] == vi) {
			EdgeIndex e2(fi[2], fi[0]);
			if(edges[e2].isOnBorder()) 
				return true;
		}
	}
	return false; 
}

void EdgeCollapse::getCollapsedFaces(std::vector<int> &cfv,
			const std::vector<int> &faces,
			int va, int vb)
{
	const unsigned *indices = m_mesh->c_indices();
	std::vector<int>::const_iterator it = faces.begin();
	for(;it!=faces.end();++it) {
		const unsigned *fi = &indices[*it * 3];

		int fv[3];
		fv[0] = fi[0];
		fv[1] = fi[1];
		fv[2] = fi[2];

		if(fv[0] == vb || fv[1] == vb || fv[2] == vb) 
			continue;

		if(fv[0] == va) fv[0] = vb;
		if(fv[1] == va) fv[1] = vb;
		if(fv[2] == va) fv[2] = vb;

		cfv.push_back(fv[0]);
		cfv.push_back(fv[1]);
		cfv.push_back(fv[2]);
	}
}

void EdgeCollapse::removeConnectedEdges(std::map<EdgeIndex, EdgeValue> &edges, 
        const VertexValue &vert, int vi)
{
	std::vector<int> neivs;
	vert.getConnectedVertices(neivs, vi, m_mesh->c_indices());
	std::vector<int>::const_iterator it = neivs.begin();
	for(;it!=neivs.end();++it) {
		std::map<EdgeIndex, EdgeValue>::iterator fe = edges.find(EdgeIndex(vi, *it));
		if(fe != edges.end())
			edges.erase(fe);
		else
			std::cout<< "\n\n cannot find e "<<EdgeIndex(vi, *it);
	}
}

void EdgeCollapse::disconnectedFaces(std::vector<VertexValue> &vertices,
        const VertexValue &vert, int vi)
{
	std::vector<int> neivs;
	vert.getConnectedVertices(neivs, vi, m_mesh->c_indices());
	std::vector<int>::const_iterator it = neivs.begin();
	for(;it!=neivs.end();++it)
		vertices[*it].removeFaceConnectedTo(vi, m_mesh->c_indices());
}

void EdgeCollapse::addFaceEdges(std::map<EdgeIndex, EdgeValue> &edges,
        const std::vector<int> &faceVertices)
{
	for(int i=0;i<faceVertices.size();i+=3) {
		EdgeIndex e0(faceVertices[i], faceVertices[i+1]);
		if(edges.find(e0) == edges.end()) {
			edges[e0] = EdgeValue();
			std::cout << " add edge " << e0;
		}

		EdgeIndex e1(faceVertices[i+1], faceVertices[i+2]);
		if(edges.find(e1) == edges.end()) {
			edges[e1] = EdgeValue();
			std::cout << " add edge " << e1;
		}

		EdgeIndex e2(faceVertices[i+2], faceVertices[i]);
		if(edges.find(e2) == edges.end()) {
			edges[e2] = EdgeValue();
			std::cout << " add edge " << e2;
		}
	}
}

void EdgeCollapse::connectFaceToVertices(std::vector<VertexValue> &vertices,
        const std::vector<int> &faceVertices,
        int firstFaceInd)
{
	for(int i=0;i<faceVertices.size();i+=3) {
		int fi = firstFaceInd + i / 3;
		vertices[faceVertices[i]].connectToFace(fi);
		vertices[faceVertices[i+1]].connectToFace(fi);
		vertices[faceVertices[i+2]].connectToFace(fi);
	}
}

void EdgeCollapse::updateCost(std::vector<VertexValue> &vertices,
        const std::vector<int> &faceVertices)
{
	const unsigned *ind = m_mesh->c_indices();
	const Vector3F *nmls = m_mesh->c_normals();
	for(int i=0;i<faceVertices.size();i+=3) {
		vertices[faceVertices[i]].computeCost(i, ind, nmls);
		vertices[faceVertices[i+1]].computeCost(i, ind, nmls);
		vertices[faceVertices[i+2]].computeCost(i, ind, nmls);
	}

	for(int i=0;i<faceVertices.size();i+=3) {
		computeEdgeCost(EdgeIndex(faceVertices[i], faceVertices[i + 1]), vertices);
		computeEdgeCost(EdgeIndex(faceVertices[i + 1], faceVertices[i + 2]), vertices);
		computeEdgeCost(EdgeIndex(faceVertices[i + 2], faceVertices[i]), vertices);
	}
}

void EdgeCollapse::relocateFacesTo(const std::vector<int> &faces, int toLastFace,
        std::vector<VertexValue> &vertices)
{
	int toFace = toLastFace;
	std::vector<int>::const_iterator it = faces.begin();

    for(;it!=faces.end();++it) {
    	std::cout<<"\n swap f " << *it << " to "<<toFace;
    	int fv[3];
    	fv[0] = m_mesh->c_indices()[*it * 3];
    	fv[1] = m_mesh->c_indices()[*it * 3 + 1];
    	fv[2] = m_mesh->c_indices()[*it * 3 + 2];

    	
    	std::cout<<"\n v"<<fv[0];
    	vertices[fv[0]].swapFace(*it, toFace);
    	std::cout<<"\n v"<<fv[1];
    	vertices[fv[1]].swapFace(*it, toFace);
    	std::cout<<"\n v"<<fv[2];
    	vertices[fv[2]].swapFace(*it, toFace);

    	std::cout<<"\n swap f " << toFace << " to "<< *it;
    	fv[0] = m_mesh->c_indices()[toFace * 3];
    	fv[1] = m_mesh->c_indices()[toFace * 3 + 1];
    	fv[2] = m_mesh->c_indices()[toFace * 3 + 2];

    	std::cout<<"\n v"<<fv[0];
    	vertices[fv[0]].swapFace(toFace, *it);
    	std::cout<<"\n v"<<fv[1];
    	vertices[fv[1]].swapFace(toFace, *it);
    	std::cout<<"\n v"<<fv[2];
    	vertices[fv[2]].swapFace(toFace, *it);

    	m_mesh->swapFace(*it, toFace);

/// move forward
    	toFace--;
    }
}

}
