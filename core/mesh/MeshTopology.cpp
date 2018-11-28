#include "MeshTopology.h"
#include <geom/V1ATriangleMesh.h>
#include "VertexValue.h"

namespace alo {

MeshTopology::MeshTopology()
{}

MeshTopology::~MeshTopology()
{ 
	delete[] m_vertices; 
}

const int &MeshTopology::numVertices() const
{ return m_numVertices; }

const int &MeshTopology::numFaces() const
{ return m_numFaces; }

const int &MeshTopology::numBorderVertices() const
{ return m_numBorderVertices; }

void MeshTopology::buildTopology(const ver1::ATriangleMesh *mesh)
{
	m_numFaces = mesh->numTriangles();
	m_numVertices = mesh->numVertices();
	m_vertices = new VertexValue[m_numVertices];

	for(int i=0;i<m_numFaces;++i) {
		const Int3 &fv = mesh->c_indices()[i];
		addEdge(EdgeIndex(fv.x, fv.y));
		addEdge(EdgeIndex(fv.y, fv.z));
		addEdge(EdgeIndex(fv.z, fv.x));
	}

	const Vector3F *pos = mesh->c_positions();

	bool stat;
	for(int i=0;i<mesh->numIndices();i+=3) {
		const int j = i/3;
		const Int3 &fv = mesh->c_indices()[j];
		FaceIndex fi(fv.x, fv.y, fv.z);
		FaceValue *f = m_tris.insert(fi, FaceValue(j) );
		f->setVertexUV(fv.x, i, fv.y, i + 1, fv.z, i + 2);
		f->setArea(mesh->getTriangleArea(j));
		f->setNormal(mesh->getTriangleNormal(j));

		EdgeValue *e0 = m_edges.find(EdgeIndex(fv.x, fv.y));
		stat = e0->connectToFace(fi);
		PrintUnmanifoldEdgeWarning(fi, *e0, stat);
	    
		EdgeValue *e1 = m_edges.find(EdgeIndex(fv.y, fv.z));
		stat = e1->connectToFace(fi);
		PrintUnmanifoldEdgeWarning(fi, *e1, stat);
		
		EdgeValue *e2 = m_edges.find(EdgeIndex(fv.z, fv.x));
		stat = e2->connectToFace(fi);
		PrintUnmanifoldEdgeWarning(fi, *e2, stat);
		
		m_vertices[fv.x].connectToFace(fi);
		m_vertices[fv.y].connectToFace(fi);
		m_vertices[fv.z].connectToFace(fi);
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
		<<" n edge "<<numEdges()
		<<" n vertex-on-border "<<m_numBorderVertices;
}

bool MeshTopology::checkTopology(const ver1::ATriangleMesh *mesh)
{
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
			int find = m_tris.find(fi)->ind();
			//int find = m_tris[fi].ind();
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

void MeshTopology::setMeshFaceInd(const ver1::ATriangleMesh *mesh, int i)
{
	const Int3 &fv = mesh->c_indices()[i];
	FaceIndex fi(fv.x, fv.y, fv.z);
	setFaceInd(fi, i);
}

const VertexValue &MeshTopology::vertex(int i) const
{ return m_vertices[i]; }

VertexValue &MeshTopology::vertex(int i)
{ return m_vertices[i]; }

EdgeValue *MeshTopology::edge(const EdgeIndex &ei)
{ return m_edges.find(ei); }

FaceValue &MeshTopology::face(const FaceIndex &fi)
{ return *m_tris.find(fi); }

void MeshTopology::addPastFace(const FaceIndex &fi, const int &i)
{
	int *p = m_pastFaceInds.find(fi);
	if(p) *p = i;
	else m_pastFaceInds.insert(fi, i);
}

void MeshTopology::addFace(const FaceIndex &fi, const FaceValue &f)
{
	//if(m_tris.find(fi) == m_tris.end())
	//	m_tris[fi] = f;
	m_tris.insert(fi, f);
}

void MeshTopology::addEdge(const EdgeIndex &e)
{
	//if(m_edges.find(e) == m_edges.end())
	//	m_edges[e] = EdgeValue();
	if(!m_edges.find(e))
		m_edges.insert(e, EdgeValue());
}

bool MeshTopology::removePastFace(const FaceIndex &fi)
{
	//std::map<FaceIndex, int>::iterator ff = m_pastFaceInds.find(fi);
	//if(ff == m_pastFaceInds.end()) {
	//if(!m_pastFaceInds.findKey(fi)) {
	//	std::cout<< "\n\n ERROR cannot remove nonexistent past face "<<fi;
	//	return false;
	//}

	//m_pastFaceInds.erase(ff);
	m_pastFaceInds.remove(fi);
	return true;
}

bool MeshTopology::removeFace(const FaceIndex &fi)
{ 
	//std::map<FaceIndex, FaceValue>::iterator ff = m_tris.find(fi);
	//if(ff == m_tris.end()) {
	//	std::cout<< "\n\n ERROR cannot remove nonexistent face "<<fi;
	//	return false;
	//}

	//m_tris.erase(ff);
	m_tris.remove(fi);
	return true;
}

bool MeshTopology::removeEdge(const EdgeIndex &ei)
{
	//std::map<EdgeIndex, EdgeValue>::iterator fe = m_edges.find(ei);
	//if(fe == m_edges.end()) {
	//	std::cout<< "\n\n ERROR cannot remove nonexistent edge "<<ei;
	//	return false;
	//}
	
	//m_edges.erase(fe);
	m_edges.remove(ei);
	return true;
}

void MeshTopology::reformFaces(std::deque<FaceIndex> &inds,
                    std::deque<FaceValue> &uvs,
                    int va, int vb)
{
	std::deque<FaceIndex>::iterator it = inds.begin();
	for(;it!=inds.end();++it) {
		uvs.push_back(face(*it));
		uvs.back().replaceVertex(va, vb);
		it->reform(va, vb);
	}
}

bool MeshTopology::addFaces(const std::deque<FaceIndex> &faces, 
					const std::deque<FaceValue> &uvs,
					int lastV)
{
    bool stat;
	std::deque<FaceIndex>::const_iterator it = faces.begin();
	for(int i=0;it!=faces.end();++it,++i) {
		const FaceIndex &fi = *it;

/// no edge connection to past
		const bool isInRange = fi.v2() < lastV;

		if(isInRange)
			addFace(fi, uvs[i]);
		else
			addPastFace(fi, uvs[i].ind());
		
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
		
  	try {
/// disconnect valence edges
        EdgeIndex ei0(fi.v0(), fi.v1());
        if(!ei0.hasV(vi)) {
            EdgeValue *e = edge(ei0);
            e->disconnectFace(fi);
        }
       
        EdgeIndex ei1(fi.v1(), fi.v2());
        if(!ei1.hasV(vi)) {
            EdgeValue *e = edge(ei1);
            e->disconnectFace(fi);
        }
       
        EdgeIndex ei2(fi.v0(), fi.v2());
        if(!ei2.hasV(vi)) {
            EdgeValue *e = edge(ei2);
            e->disconnectFace(fi);
        }

	} catch (...) {
		std::cout << " removeVertexConnection err " << vi;
    }

	}

	vert.clearFaces();
	return true;
}

bool MeshTopology::faceExists(const FaceIndex &fi)
{ return m_tris.find(fi) != NULL; }

bool MeshTopology::pastFaceExists(const FaceIndex &fi)
{ return m_pastFaceInds.find(fi) != NULL; }

bool MeshTopology::isVertexOnBorder(int vi, const VertexValue &vert,
								const ver1::ATriangleMesh *mesh)
{ 
    if(vert.numConnectedFaces() < 3) return true;

	std::deque<FaceIndex>::const_iterator it = vert.facesBegin();
	for(;it!=vert.facesEnd();++it) {
		const FaceIndex &fi = *it;

		if(fi.v0() == vi || fi.v1() == vi) {
			EdgeIndex e0(fi.v0(), fi.v1());
			if(edge(e0)->isOnBorder()) 
				return true;
		}

		if(fi.v1() == vi || fi.v2() == vi) {
			EdgeIndex e1(fi.v1(), fi.v2());
			if(edge(e1)->isOnBorder()) 
				return true;
		}

		if(fi.v2() == vi || fi.v0() == vi) {
			EdgeIndex e2(fi.v2(), fi.v0());
			if(edge(e2)->isOnBorder()) 
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

void MeshTopology::replaceMeshVertex(ver1::ATriangleMesh *mesh, 
					const FaceIndex &fi, int va, int vb)
{
	int *pti = m_pastFaceInds.find(fi);
	int ti = *pti;//m_pastFaceInds[fi];
	//if(!mesh->checkFaceVertex(ti, fi.v0(), fi.v1(), fi.v2()) ) {
	//	std::cout<<"\n past face "<<fi<<" missd "<<ti;
	//}
	mesh->replaceFaceVertex(ti, va, vb);

	m_vertices[fi.v0()].disconnectPastFace(fi);
	m_vertices[fi.v1()].disconnectPastFace(fi);
	m_vertices[fi.v2()].disconnectPastFace(fi);
	removePastFace(fi);

	
	FaceIndex rfi = fi.reformed(va, vb);

	//if(pastFaceExists(rfi)) {
	//	std::cout<<"\n WARNING past already exists "<<ti
	//			<<" replace "<<fi<<" to "<<rfi
	//			<<" a "<<va<<" b "<<vb;
	//}
	addPastFace(rfi, ti);
	//m_pastFaceInds[rfi] = ti;
	
	m_vertices[rfi.v0()].connectToPastFace(rfi);
	m_vertices[rfi.v1()].connectToPastFace(rfi);
	m_vertices[rfi.v2()].connectToPastFace(rfi);
}

void MeshTopology::pushPastFaceIndex(int x)
{
	//std::map<FaceIndex, int>::iterator it = m_pastFaceInds.begin();
	//for(;it!=m_pastFaceInds.end();++it)
	//	it->second +=x;
	m_pastFaceInds.add(x);
}

MeshTopology::EdgeListType::iterator MeshTopology::edgesBegin()
{ return m_edges.begin(); }

MeshTopology::EdgeListType::iterator MeshTopology::edgesEnd()
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
	edge(ei)->connectToFace(fi);
	//PrintUnmanifoldEdgeWarning(fi, edge(ei), stat);
}

void MeshTopology::indexPastFaces(const ver1::ATriangleMesh *mesh, int begin, int end)
{
	for(int i=begin;i<end;++i) {
		const Int3 &fv = mesh->c_indices()[i];
		FaceIndex fi(fv.x, fv.y, fv.z);
		if(!setPastFaceInd(fi, i) )
			std::cout << "\n\n WARNING nonexistent past face " << fi;
	}
}

bool MeshTopology::setFaceInd(const FaceIndex &fi, int x)
{
	FaceValue *f = m_tris.find(fi);
	if(!f) return false;
	f->setInd(x);
	//if(!faceExists(fi))
	//	return false;
	//m_tris[fi].ind() = x;
	return true;
}

bool MeshTopology::setPastFaceInd(const FaceIndex &fi, int x)
{
	//if(!pastFaceExists(fi))
	//	return false;
	int *i = m_pastFaceInds.find(fi);
	*i = x;
	//m_pastFaceInds[fi] = x;
	return true;
}

void MeshTopology::getFaceInds(std::vector<int> &faceInds,
                		const std::deque<FaceValue> &faces) const
{
	std::deque<FaceValue>::const_iterator it = faces.begin();
	for(;it!=faces.end();++it) 
		faceInds.push_back(it->ind());
}

int MeshTopology::numEdges() const
{ return m_edges.size(); }

void MeshTopology::indexEdges()
{
	//std::map<EdgeIndex, EdgeValue>::iterator it = edgesBegin();
	//for(int i=0;it!=edgesEnd();++it,++i)
	//	it->second.setInd(i);
	int acc = 0;
	EdgeListType::iterator it = edgesBegin();
	for(;it!=edgesEnd();++it) {
		EdgeDataType *block = *it;

		for(int i=0;i<block->count();++i) {
			EdgeValue &e = block->value(i);
			e.setInd(acc);
			acc++;
		}
	}
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
