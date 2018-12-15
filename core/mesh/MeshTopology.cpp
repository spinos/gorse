/*
 *  MeshTopology.cpp
 *  aloe
 *
 */

#include "MeshTopology.h"
#include <geom/V1ATriangleMesh.h>
#include "VertexValue.h"

namespace alo {

MeshTopology::MeshTopology()
{}

MeshTopology::~MeshTopology()
{}

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
	m_vertices.resize(m_numVertices);
	m_vertexFaceConnection.resize(m_numVertices);
	m_vertexPastFaceConnection.resize(m_numVertices);

	for(int i=0;i<m_numFaces;++i) {
		const Int3 &fv = mesh->c_indices()[i];
		addEdge(EdgeIndex(fv.x, fv.y));
		addEdge(EdgeIndex(fv.y, fv.z));
		addEdge(EdgeIndex(fv.z, fv.x));
	}

	const Vector3F *pos = mesh->c_positions();
	const Vector3F *nml = mesh->c_normals();

	bool stat;
	for(int i=0;i<m_numFaces;++i) {
		if((i+1 & 4095) == 0) std::cout << ".";
		const int i3 = i * 3;
		const Int3 &fv = mesh->c_indices()[i];
		FaceIndex fi(fv.x, fv.y, fv.z);
		FaceValue *f = m_tris.insert(fi, FaceValue(i) );
		f->setVertexUV(fv.x, i3, fv.y, i3 + 1, fv.z, i3 + 2);
		f->setArea(mesh->getTriangleArea(i));
		f->setNormal(mesh->getTriangleNormal(i));

		EdgeValue *e0 = m_edges.find(EdgeIndex(fv.x, fv.y));
		stat = e0->connectToFace(fi);
		PrintUnmanifoldEdgeWarning(fi, *e0, stat);
	    
		EdgeValue *e1 = m_edges.find(EdgeIndex(fv.y, fv.z));
		stat = e1->connectToFace(fi);
		PrintUnmanifoldEdgeWarning(fi, *e1, stat);
		
		EdgeValue *e2 = m_edges.find(EdgeIndex(fv.z, fv.x));
		stat = e2->connectToFace(fi);
		PrintUnmanifoldEdgeWarning(fi, *e2, stat);
		
		m_vertexFaceConnection.connectFace(fi);
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

		bool ringStat = checkConcaveRing(i, pos, nml[i]);
		vi.setRingConcave(ringStat);
	}
}

bool MeshTopology::checkTopology(const ver1::ATriangleMesh *mesh)
{
	const int &nv = mesh->numVertices();
	for(int i=0;i<nv;++i) {
		if(!m_vertexFaceConnection.check(i)) return false;

		const std::deque<FaceIndex> &faces = m_vertexFaceConnection[i];
		std::deque<FaceIndex>::const_iterator it = faces.begin();
		for(;it!=faces.end();++it) {
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

FaceValue *MeshTopology::face(const FaceIndex &fi)
{ return m_tris.find(fi); }

void MeshTopology::addPastFace(const FaceIndex &fi, const int &i)
{
	int *p = m_pastFaceInds.find(fi);
	if(p) *p = i;
	else m_pastFaceInds.insert(fi, i);
}

FaceValue *MeshTopology::addFace(const FaceIndex &fi, const FaceValue &f)
{
	return m_tris.insert(fi, f);
}

EdgeValue *MeshTopology::addEdge(const EdgeIndex &e)
{
	return m_edges.insert(e, EdgeValue(), false);
}

bool MeshTopology::removePastFace(const FaceIndex &fi)
{
	m_pastFaceInds.remove(fi);
	return true;
}

bool MeshTopology::removeFace(const FaceIndex &fi)
{
	m_tris.remove(fi);
	return true;
}

bool MeshTopology::removeEdge(const EdgeIndex &ei)
{
	m_edges.remove(ei);
	return true;
}

void MeshTopology::reformFaces(std::deque<FaceIndex> &inds,
                    std::deque<FaceValue> &uvs,
                    int va, int vb)
{
	std::deque<FaceIndex>::iterator it = inds.begin();
	for(;it!=inds.end();++it) {
		uvs.push_back(*face(*it));
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
		EdgeValue *ev0 = addEdge(e0);
		if(isInRange) {
		    stat = ev0->connectToFace(fi);
		    PrintUnmanifoldEdgeWarning(fi, *ev0, stat);
		    if(!stat) return false;
		}
		
		EdgeIndex e1(fi.v1(), fi.v2());
		EdgeValue *ev1 = addEdge(e1);
		if(isInRange) {
		    stat = ev1->connectToFace(fi);
		    PrintUnmanifoldEdgeWarning(fi, *ev1, stat);
		    if(!stat) return false;
		}
		
		EdgeIndex e2(fi.v2(), fi.v0());
		EdgeValue *ev2 = addEdge(e2);
		if(isInRange) {
		    stat = ev2->connectToFace(fi);
		    PrintUnmanifoldEdgeWarning(fi, *ev2, stat);
		    if(!stat) return false;
		}
		
		if(isInRange)
			m_vertexFaceConnection.connectFace(fi);
		else 
			m_vertexPastFaceConnection.connectFace(fi);
			
	}
	return true;
}

bool MeshTopology::removeVertexEdgeFace(int vi)
{
	std::deque<int> neivs;
	getConnectedVertices(neivs, vi);
	std::deque<int>::const_iterator it = neivs.begin();
	for(;it!=neivs.end();++it) 
		removeEdge(EdgeIndex(vi, *it));	

	const std::deque<FaceIndex> &faces = facesConnectedTo(vi);
	std::deque<FaceIndex>::const_iterator fit = faces.begin();
	for(;fit!= faces.end();++fit) {
		const FaceIndex &fi = *fit;

		removeFace(fi);
/// disconnect valence vertices
		if(fi.v0() != vi)
			m_vertexFaceConnection.disconnectFace(fi.v0(), fi);

		if(fi.v1() != vi)
			m_vertexFaceConnection.disconnectFace(fi.v1(), fi);

		if(fi.v2() != vi)
			m_vertexFaceConnection.disconnectFace(fi.v2(), fi);
		
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

	m_vertexFaceConnection.clearFaces(vi);
	return true;
}

bool MeshTopology::faceExists(const FaceIndex &fi)
{ return m_tris.find(fi) != NULL; }

bool MeshTopology::pastFaceExists(const FaceIndex &fi)
{ return m_pastFaceInds.find(fi) != NULL; }

bool MeshTopology::isVertexOnBorder(int vi, const VertexValue &vert,
								const ver1::ATriangleMesh *mesh)
{ 
	const std::deque<FaceIndex> &faces = facesConnectedTo(vi);
    if(faces.size() < 3) return true;

	std::deque<FaceIndex>::const_iterator it = faces.begin();
	for(;it!=faces.end();++it) {
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
	const std::deque<FaceIndex> &faces = facesConnectedTo(vi);
	std::deque<FaceIndex>::const_iterator it = faces.begin();
	for(;it!= faces.end();++it) {
		const FaceValue *facei = face(*it);
		const int uvi = facei->vertexUV(vi);
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

	m_vertexPastFaceConnection.disconnectFace(fi);

	removePastFace(fi);
	
	FaceIndex rfi = fi.reformed(va, vb);

	//if(pastFaceExists(rfi)) {
	//	std::cout<<"\n WARNING past already exists "<<ti
	//			<<" replace "<<fi<<" to "<<rfi
	//			<<" a "<<va<<" b "<<vb;
	//}
	addPastFace(rfi, ti);
	
	m_vertexPastFaceConnection.connectFace(rfi);
}

void MeshTopology::pushPastFaceIndex(int x)
{
	sdb::L3Node<FaceIndex, int, 1024> *block = m_pastFaceInds.begin();
	while(block) {
	    block->add(x);
		block = m_pastFaceInds.next(block);
	}
}

MeshTopology::EdgeDataType *MeshTopology::firstEdge()
{ return m_edges.begin(); }

MeshTopology::EdgeDataType *MeshTopology::nextEdge(const EdgeDataType *x)
{ return m_edges.next(x); }

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
	return true;
}

bool MeshTopology::setPastFaceInd(const FaceIndex &fi, int x)
{
	int *i = m_pastFaceInds.find(fi);
	if(!i) return false;
	*i = x;
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
	int acc = 0;
	EdgeDataType *block = firstEdge();
	while(block) {
		for(int i=0;i<block->count();++i) {
			EdgeValue &e = block->value(i);
			e.setInd(acc);
			acc++;
		}
		block = nextEdge(block);
	}
}

void MeshTopology::copyPastFacesTo(std::vector<FaceIndex> &faces, int v) const
{ m_vertexPastFaceConnection.copyFacesTo(faces, v); }
    
void MeshTopology::clearPastFaces(int v)
{ m_vertexPastFaceConnection.clearFaces(v); }

bool MeshTopology::hasPastConnected(int a, int b) const
{ return m_vertexPastFaceConnection.hasIntersection(a, b); }

void MeshTopology::copyFacesTo(std::deque<FaceIndex> &faces, int v) const
{ faces = m_vertexFaceConnection[v]; }

bool MeshTopology::hasFaceConnected(int a, int b) const
{ return m_vertexFaceConnection.hasIntersection(a, b); }

int MeshTopology::numFacesConnectedTo(int vi) const
{ return m_vertexFaceConnection.numFacesConnected(vi); }

const std::deque<FaceIndex> &MeshTopology::facesConnectedTo(int vi) const
{ return m_vertexFaceConnection[vi]; }

bool MeshTopology::faceConnectedIsOor(int vi, int lastFace)
{
	const std::deque<FaceIndex> &vif = m_vertexFaceConnection[vi];
    const int nf = vif.size();
    const int lastF = lastFace - nf;
    std::deque<FaceIndex>::const_iterator it = vif.begin();
	for(;it!=vif.end();++it) {
		const FaceIndex &fi = *it;

		FaceValue *fv = face(fi);
		if(!fv) continue;

/// cannot connect to face to be relocated
		if(fv->ind() + nf > lastF)
		    return true;
	}
	return false;
}

void MeshTopology::getConnectedVertices(std::deque<int> &neivs, int vi) const
{ m_vertexFaceConnection.getConnectedVertices(neivs, vi); }

bool MeshTopology::getVertexOneRing(std::vector<int> &vring, int vi,
                    const Vector3F *pos,
                    const Vector3F &nml) const
{ return m_vertexFaceConnection.getVertexOneRing(vring, vi, pos, nml); }

bool MeshTopology::checkConcaveRing(int vi, 
                    const Vector3F *pos,
                    const Vector3F &nml)
{ 
	std::vector<int> vring;
	bool stat = m_vertexFaceConnection.getVertexOneRing(vring, vi, pos, nml);
	if(!stat) return false;
	
	if(checkInnerEdges(vring, 2)) return true;
	
	if(vring.size() < 6) return false;
	
	if(checkInnerEdges(vring, 3)) return true;
	
	return false;
}

bool MeshTopology::checkInnerEdges(const std::vector<int> &vring, int jump)
{
	const int n = vring.size();
	for(int i=0;i<n;++i) {
		int v0 = vring[i];
		int v1 = vring[(i+jump) % n];
		if(m_edges.find(EdgeIndex(v0, v1))) return true;
	}
	return false; 
}

void MeshTopology::expandVertexRing(std::vector<int> &bigRing,
                    const std::vector<int> &smallRing) const
{
	std::vector<int>::const_iterator it = smallRing.begin();
	for(;it!=smallRing.end();++it)
		m_vertexFaceConnection.addVertexRingTo(bigRing, smallRing, *it);
}

void MeshTopology::updateVertexConcaveState(const std::vector<int> &vertexInds,
					const Vector3F *pos, const Vector3F *nml)
{
	std::vector<int>::const_iterator it = vertexInds.begin();
	for(;it!=vertexInds.end();++it) {
		const int i = *it;
		VertexValue &vi = vertex(i);
		if(vi.isRingConcave()) continue;
		bool ringStat = checkConcaveRing(i, pos, nml[i]);
		vi.setRingConcave(ringStat);
	}
}

void MeshTopology::PrintUnmanifoldEdgeWarning(const FaceIndex &fi, const EdgeValue &e,
                bool stat)
{ 
    if(!stat) 
        std::cout << "\n WARNING avoid unmanifold edge " << e << " face " << fi; 
}

void MeshTopology::PrintAddFaceWarning(const std::deque<FaceIndex> &faces, 
                bool stat)
{
	if(stat) return;
	std::cout << "\n WARNING cannot add faces [";
	for(int i=0;i<faces.size();++i)
		std::cout << " " << faces[i];
	std::cout << "] ";
}

void  MeshTopology::printDetail() const
{
	std::cout<<"\n n vertex "<<m_numVertices
		<<" n face "<<m_numFaces
		<<" n edge "<<numEdges()
		<<" n vertex-on-border "<<m_numBorderVertices
		<<" n vertex-face-connection "<<m_vertexFaceConnection.totalSize()
		<<" n vertex-past-connection "<<m_vertexPastFaceConnection.totalSize();
}

}
