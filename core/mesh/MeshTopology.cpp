#include "MeshTopology.h"
#include <geom/ATriangleMesh.h>
#include "EdgeIndex.h"
#include "EdgeValue.h"
#include "FaceIndex.h"
#include "FaceValue.h"
#include "VertexValue.h"

namespace alo {

MeshTopology::MeshTopology()
{}

MeshTopology::~MeshTopology()
{ 
	delete[] m_vertices; 
	m_edges.clear();
	m_tris.clear();
	m_pastFaces.clear();
}

const int &MeshTopology::numVertices() const
{ return m_numVertices; }

const int &MeshTopology::numFaces() const
{ return m_numFaces; }

const int &MeshTopology::numBorderVertices() const
{ return m_numBorderVertices; }

void MeshTopology::buildTopology(const ATriangleMesh *mesh)
{
	m_numFaces = mesh->numTriangles();
	m_numVertices = mesh->numVertices();
	m_vertices = new VertexValue[m_numVertices];

	const unsigned *ind = mesh->c_indices();
	for(int i=0;i<mesh->numIndices();i+=3) {
		addEdge(EdgeIndex(ind[i], ind[i + 1]));
		addEdge(EdgeIndex(ind[i + 1], ind[i + 2]));
		addEdge(EdgeIndex(ind[i + 2], ind[i]));
	}

	const Vector3F *pos = mesh->c_positions();

	bool stat;
	for(int i=0;i<mesh->numIndices();i+=3) {
		const int j = i/3;

		FaceIndex fi(ind[i], ind[i + 1], ind[i + 2]);
		m_tris[fi] = FaceValue(j);
		FaceValue &f = m_tris[fi];
		f.setVertexUV(ind[i], i, ind[i + 1], i + 1, ind[i + 2], i + 2);
		f.setArea(mesh->getTriangleArea(j));
		f.setNormal(mesh->getTriangleNormal(j));

		EdgeValue &e0 = m_edges[EdgeIndex(ind[i], ind[i + 1])];
		stat = e0.connectToFace(fi);
		PrintUnmanifoldEdgeWarning(fi, e0, stat);
	    
		EdgeValue &e1 = m_edges[EdgeIndex(ind[i + 1], ind[i + 2])];
		stat = e1.connectToFace(fi);
		PrintUnmanifoldEdgeWarning(fi, e1, stat);
		
		EdgeValue &e2 = m_edges[EdgeIndex(ind[i + 2], ind[i])];
		stat = e2.connectToFace(fi);
		PrintUnmanifoldEdgeWarning(fi, e2, stat);
		
		m_vertices[ind[i]].connectToFace(fi);
		m_vertices[ind[i + 1]].connectToFace(fi);
		m_vertices[ind[i + 2]].connectToFace(fi);
	}

	m_numBorderVertices=0;
	for(int i=0;i<m_numVertices;++i) {
		VertexValue &vi = vertex(i);
		if(isVertexOnBorder(i, vi, mesh)) {
/// always on border
			vi.setOnBorder(true);
			m_numBorderVertices++;
		} else
			vi.setOnBorder(false);
	}

	std::cout<<"\n n vertex "<<m_numVertices
		<<" n face "<<m_numFaces
		<<" n edge "<<m_edges.size()
		<<" n vertex-on-border "<<m_numBorderVertices;
}

bool MeshTopology::checkTopology(const ATriangleMesh *mesh)
{
	const unsigned *inds = mesh->c_indices();
	const int &nv = mesh->numVertices();
	for(int i=0;i<nv;++i) {
		if(!m_vertices[i].checkFaces(i)) return false;
		std::deque<FaceIndex>::const_iterator it = m_vertices[i].facesBegin();
		for(;it!=m_vertices[i].facesEnd();++it) {
			const FaceIndex &fi = *it;
			if(!faceExists(fi)) {
				std::cout << "\n\n ERROR v "<<i<<" nonexistent face " << fi;
				return false;
			}
			int find = m_tris[fi].ind();
			if(find < 0 || find >= mesh->numTriangles() ) {
				std::cout << "\n\n ERROR v "<<i<<" invalid face ind " << find;
				return false;
			}

			if(!mesh->checkFaceVertex(find, fi.v0(), fi.v1(), fi.v2())) {

				std::cout << "\n\n ERROR v "<<i<<" mismatch face " << fi;
				mesh->printFace(find);
				return false;
			}
		}
	}

	return true;
}

void MeshTopology::setMeshFaceInd(const ATriangleMesh *mesh, int i)
{
	const unsigned *inds = mesh->c_indices();
	int fv[3];
	fv[0] = inds[i * 3];
	fv[1] = inds[i * 3 + 1];
	fv[2] = inds[i * 3 + 2];
	FaceIndex fi(fv[0], fv[1], fv[2]);
	setFaceInd(fi, i);
}

const VertexValue &MeshTopology::vertex(int i) const
{ return m_vertices[i]; }

VertexValue &MeshTopology::vertex(int i)
{ return m_vertices[i]; }

EdgeValue &MeshTopology::edge(const EdgeIndex &ei)
{ return m_edges[ei]; }

FaceValue &MeshTopology::face(const FaceIndex &fi)
{ return m_tris[fi]; }

void MeshTopology::addPastFace(const FaceIndex &fi)
{
	if(m_pastFaces.find(fi) == m_pastFaces.end())
		m_pastFaces[fi] = FaceValue();
}

void MeshTopology::addFace(const FaceIndex &fi)
{
	if(m_tris.find(fi) == m_tris.end())
		m_tris[fi] = FaceValue();
}

void MeshTopology::addEdge(const EdgeIndex &e)
{
	if(m_edges.find(e) == m_edges.end())
		m_edges[e] = EdgeValue();
		//std::cout << " add edge " << e;
}

bool MeshTopology::removePastFace(const FaceIndex &fi)
{
	std::map<FaceIndex, FaceValue>::iterator ff = m_pastFaces.find(fi);
	if(ff == m_pastFaces.end()) {
		std::cout<< "\n\n ERROR cannot remove nonexistent past face "<<fi;
		return false;
	}

	m_pastFaces.erase(ff);
	return true;
}

bool MeshTopology::removeFace(const FaceIndex &fi)
{ 
	std::map<FaceIndex, FaceValue>::iterator ff = m_tris.find(fi);
	if(ff == m_tris.end()) {
		std::cout<< "\n\n ERROR cannot remove nonexistent face "<<fi;
		return false;
	}

	m_tris.erase(ff);
	return true;
}

bool MeshTopology::removeEdge(const EdgeIndex &ei)
{
	std::map<EdgeIndex, EdgeValue>::iterator fe = m_edges.find(ei);
	if(fe == m_edges.end()) {
		std::cout<< "\n\n ERROR cannot remove nonexistent edge "<<ei;
		return false;
	}
	
	m_edges.erase(fe);
	return true;
}

bool MeshTopology::addFaces(const std::deque<FaceIndex> &faces, int lastV)
{
    bool stat;
	std::deque<FaceIndex>::const_iterator it = faces.begin();
	for(;it!=faces.end();++it) {
		const FaceIndex &fi = *it;

/// no edge connection to past
		const bool isInRange = fi.v2() < lastV;

		if(isInRange)
			addFace(fi);
		else
			addPastFace(fi);
		
		EdgeIndex e0(fi.v0(), fi.v1());
		addEdge(e0);
		if(isInRange) {
		    connectFaceToEdge(e0, fi, stat);
		    if(!stat) return false;
		}
		
		EdgeIndex e1(fi.v1(), fi.v2());
		addEdge(e1);
		if(isInRange) {
		    connectFaceToEdge(e1, fi, stat);
		    if(!stat) return false;
		}
		
		EdgeIndex e2(fi.v2(), fi.v0());
		addEdge(e2);
		if(isInRange) {
		    connectFaceToEdge(e2, fi, stat);
		    if(!stat) return false;
		}
		
		if(isInRange)
			connectFaceToVertex(fi);
		else 
			connectFaceToVertexPast(fi);
			
	}
	return true;
}

bool MeshTopology::removeVertexConnection(int vi)
{
	VertexValue &vert = m_vertices[vi];

	std::deque<int> neivs;
	vert.getConnectedVertices(neivs, vi);
	std::deque<int>::const_iterator it = neivs.begin();
	for(;it!=neivs.end();++it) 
		removeEdge(EdgeIndex(vi, *it));

	std::deque<FaceIndex>::const_iterator fit = vert.facesBegin();
	for(;fit!= vert.facesEnd();++fit) {
		const FaceIndex &fi = *fit;

		removeFace(fi);
/// disconnect valence vertices
		if(fi.v0() != vi)
			vertex(fi.v0()).disconnectFace(fi);

		if(fi.v1() != vi)
			vertex(fi.v1()).disconnectFace(fi);

		if(fi.v2() != vi)
			vertex(fi.v2()).disconnectFace(fi);
		
/// disconnect valence edges
        EdgeIndex ei0(fi.v0(), fi.v1());
        if(!ei0.hasV(vi)) {
            edge(ei0).disconnectFace(fi);
        }
        
        EdgeIndex ei1(fi.v1(), fi.v2());
        if(!ei1.hasV(vi)) {
            edge(ei1).disconnectFace(fi);
        }
        
        EdgeIndex ei2(fi.v2(), fi.v0());
        if(!ei2.hasV(vi)) {
            edge(ei2).disconnectFace(fi);
        }
	}
	vert.clearFaces();
	return true;
}

void MeshTopology::getConnectedFaceInds(std::vector<int> &faceInds,
                const VertexValue &vert)
{
	std::deque<FaceIndex>::const_iterator it = vert.facesBegin();
	for(;it!=vert.facesEnd();++it) {
		const FaceIndex &fi = *it;

		if(m_tris.find(fi) == m_tris.end()) {
			std::cout << "\n\n ERROR getConnectedFaceInd nonexistent face " << fi;
			return;
		}

		faceInds.push_back(face(fi).ind());
	}
}

bool MeshTopology::faceExists(const FaceIndex &fi)
{ return m_tris.find(fi) != m_tris.end(); }

bool MeshTopology::pastFaceExists(const FaceIndex &fi)
{ return m_pastFaces.find(fi) != m_pastFaces.end(); }

bool MeshTopology::isVertexOnBorder(int vi, const VertexValue &vert,
								const ATriangleMesh *mesh)
{ 
    if(vert.numConnectedFaces() < 3) return true;
	std::deque<FaceIndex>::const_iterator it = vert.facesBegin();
	for(;it!=vert.facesEnd();++it) {
		const FaceIndex &fi = *it;

		if(fi.v0() == vi || fi.v1() == vi) {
			EdgeIndex e0(fi.v0(), fi.v1());
			if(edge(e0).isOnBorder()) 
				return true;
		}

		if(fi.v1() == vi || fi.v2() == vi) {
			EdgeIndex e1(fi.v1(), fi.v2());
			if(edge(e1).isOnBorder()) 
				return true;
		}

		if(fi.v2() == vi || fi.v0() == vi) {
			EdgeIndex e2(fi.v2(), fi.v0());
			if(edge(e2).isOnBorder()) 
				return true;
		}
	}

	const int nuv = mesh->numUVSets();
	if(nuv < 1) return false;

	for(int i=0;i<nuv;++i) {
		const Float2 *uvs = mesh->c_uvSet(i);
		if(isVertexOnUVBorder(vi, vert, uvs)) 
			return true;
	}

	return false; 
}

/// face-varying uv has different coordinates
bool MeshTopology::isVertexOnUVBorder(int vi, const VertexValue &vert,
                const Float2 *uvs)
{
	Float2 preUV;
	bool isFirst = true;
	std::deque<FaceIndex>::const_iterator it = vert.facesBegin();
	for(;it!=vert.facesEnd();++it) {
		const FaceValue &facei = face(*it);
		const int uvi = facei.vertexUV(vi);
		const Float2 &uv = uvs[uvi];

		if(isFirst) {
			isFirst = false;		
		} else {
			if(!preUV.isCloseTo(uv)) return true;
		}

		preUV = uv;
	}
	return false;
}

void MeshTopology::lockVertices(const std::vector<int> &v)
{
	std::vector<int>::const_iterator it = v.begin();
	for(;it!=v.end();++it)
		m_vertices[*it].lock();
}

void MeshTopology::lockFaces(const std::deque<FaceIndex> &faces)
{
    std::deque<FaceIndex>::const_iterator it = faces.begin();
	for(;it!=faces.end();++it)
		lockFace(*it);
}

bool MeshTopology::lockFace(const FaceIndex &fi)
{
	if(!faceExists(fi)) return false;
	m_vertices[fi.v0()].lock();
	m_vertices[fi.v1()].lock();
	m_vertices[fi.v2()].lock();
	return true;
}

void MeshTopology::unlockVertices(int high)
{
	for(int i=0;i<high;++i)
		m_vertices[i].unlock();
}

void MeshTopology::replaceMeshVertex(ATriangleMesh *mesh, 
					const FaceIndex &fi, int va, int vb)
{
	int ti = m_pastFaces[fi].ind();
	//if(!mesh->checkFaceVertex(ti, fi.v0(), fi.v1(), fi.v2()) ) {
	//	std::cout<<"\n past face "<<fi<<" missd "<<ti;
	//}
	mesh->replaceFaceVertex(ti, va, vb);

	m_vertices[fi.v0()].disconnectPastFace(fi);
	m_vertices[fi.v1()].disconnectPastFace(fi);
	m_vertices[fi.v2()].disconnectPastFace(fi);
	removePastFace(fi);

	
	FaceIndex rfi = fi.reformed(va, vb);

	if(pastFaceExists(rfi)) {
		std::cout<<"\n WARNING past already exists "<<ti
				<<" replace "<<fi<<" to "<<rfi
				<<" a "<<va<<" b "<<vb;
	}
	addPastFace(rfi);
	m_pastFaces[rfi].ind() = ti;
	
	m_vertices[rfi.v0()].connectToPastFace(rfi);
	m_vertices[rfi.v1()].connectToPastFace(rfi);
	m_vertices[rfi.v2()].connectToPastFace(rfi);
}

void MeshTopology::pushPastFaceIndex(int x)
{
	std::map<FaceIndex, FaceValue>::iterator it = m_pastFaces.begin();
	for(;it!=m_pastFaces.end();++it)
		it->second.ind() +=x;
}

std::map<EdgeIndex, EdgeValue>::iterator MeshTopology::edgesBegin()
{ return m_edges.begin(); }

std::map<EdgeIndex, EdgeValue>::iterator MeshTopology::edgesEnd()
{ return m_edges.end(); }

void MeshTopology::connectFaceToVertex(const FaceIndex &fi)
{
	m_vertices[fi.v0()].connectToFace(fi);
	m_vertices[fi.v1()].connectToFace(fi);
	m_vertices[fi.v2()].connectToFace(fi);
}

void MeshTopology::connectFaceToVertexPast(const FaceIndex &fi)
{
	m_vertices[fi.v0()].connectToPastFace(fi);
	m_vertices[fi.v1()].connectToPastFace(fi);
	m_vertices[fi.v2()].connectToPastFace(fi);
}

void MeshTopology::connectFaceToEdge(const EdgeIndex &ei, const FaceIndex &fi, bool &stat)
{
	stat = m_edges[ei].connectToFace(fi);
	PrintUnmanifoldEdgeWarning(fi, edge(ei), stat);
}

void MeshTopology::indexPastFaces(const ATriangleMesh *mesh, int begin, int end)
{
	for(int i=begin;i<end;++i) {
		const unsigned *ind = &mesh->c_indices()[i*3];
		FaceIndex fi(ind[0], ind[1], ind[2]);
		if(!setPastFaceInd(fi, i) )
			std::cout << "\n\n WARNING nonexistent past face " << fi;
	}
}

bool MeshTopology::setFaceInd(const FaceIndex &fi, int x)
{
	if(!faceExists(fi))
		return false;
	m_tris[fi].ind() = x;
	return true;
}

bool MeshTopology::setPastFaceInd(const FaceIndex &fi, int x)
{
	if(!pastFaceExists(fi))
		return false;
	m_pastFaces[fi].ind() = x;
	return true;
}

void MeshTopology::PrintUnmanifoldEdgeWarning(const FaceIndex &fi, const EdgeValue &e,
                bool stat)
{ 
    if(!stat) 
        std::cout << "\n\n WARNING avoid unmanifold edge " << e << " face " << fi; 
}

void MeshTopology::PrintAddFaceWarning(const std::deque<FaceIndex> &faces, 
                bool stat)
{
	if(stat) return;
	std::cout << "\n\n WARNING cannot add faces [";
	for(int i=0;i<faces.size();++i)
		std::cout << " " << faces[i];
	std::cout << "] ";
}

}
