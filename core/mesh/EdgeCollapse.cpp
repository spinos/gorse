#include "EdgeCollapse.h"
#include "EdgeIndex.h"
#include "EdgeValue.h"
#include "FaceIndex.h"
#include "FaceValue.h"
#include "VertexValue.h"
#include "PolygonTriangulation.h"
#include "HistoryMesh.h"

namespace alo {

EdgeCollapse::EdgeCollapse()
{ m_triangulate = new PolygonTriangulation; }

EdgeCollapse::~EdgeCollapse()
{ 
	delete[] m_vertices; 
	delete m_triangulate;
	m_edges.clear();
	m_tris.clear();
}

void EdgeCollapse::simplify(HistoryMesh *msh)
{
	m_mesh = msh;
	m_mesh->initHistory();

	m_triangulate->setPositions(m_mesh->c_positions());
	
	buildTopology();

	computeEdgeCost();

	for(int istage = 0;istage < m_mesh->maxNumStages();++istage) {
		m_mesh->addHistoryStage();
		int ncoarse = 0, nfine = 0;
		int dnf = processStage(ncoarse, nfine);
		m_mesh->finishHistoryStage(ncoarse, nfine);
		if(dnf < 1 || canEndProcess()) break;
		unlockAllVertices();
	}

	m_mesh->finishHistory(m_numVertices, m_numFaces);
}

int EdgeCollapse::processStage(int &numCoarseFaces, int &numFineFaces)
{
	int nvStageBegin = m_mesh->numVertices();
	const int nsteps = nvStageBegin>>1;
	int i = 0;
	for(;i<nsteps;++i) {
		EdgeIndex collapseEdgeI = findEdgeToCollapse();
		if(!collapseEdgeI.isValid()) break;

		int vert2Remove, vert2Keep;
		getVertexToRemove(vert2Remove, vert2Keep, collapseEdgeI);
		const int lastV = m_mesh->numVertices() - 1;

		const VertexValue &vrm = m_vertices[vert2Remove];
		
		std::vector<int> &vring = m_triangulate->vertices();
		vring.clear();
		if(!vrm.getOneRing(vring, vert2Remove,
			m_mesh->c_positions(),
			m_mesh->c_normals()[vert2Remove])) {

			std::cout << "\n\n ERROR not one ring v" << vert2Remove << vrm;
			return -1;
		}

/// collapsed faces to be created
		std::deque<FaceIndex> collapsedFaces;
		if(!m_triangulate->getTriangles(collapsedFaces)) {
			std::cout << "\n\n ERROR wrong triangulate n face" << collapsedFaces.size();
			return -1;
		}

		std::deque<FaceIndex> lastFaces;
		std::deque<FaceIndex> reducedFaces;
		std::vector<int> vaFaceInds;
		std::vector<int> vbFaceInds;
		
		vrm.getReformedFaces(reducedFaces, vert2Remove, lastV);

/// to be connected to relocated vertex
		getConnectedFaceInds(vaFaceInds, vrm);
		getConnectedFaceInds(vbFaceInds, m_vertices[lastV]);
		m_vertices[lastV].getReformedFaces(lastFaces, lastV, vert2Remove);

/// remove faces connected to vertex 
		if(!removeFaces(vrm, vert2Remove)) {
			PrintCollapseEdgeError(vert2Remove, lastV,
				m_vertices[vert2Remove], m_vertices[lastV]);
			return -1;
		}
		m_vertices[vert2Remove].clearFaces();

/// remove faces connected to last vertex
		if(!removeFaces(m_vertices[lastV], lastV)) {
			PrintCollapseEdgeError(vert2Remove, lastV,
				m_vertices[vert2Remove], m_vertices[lastV]);
			return -1;
		} 
		m_vertices[lastV].clearFaces();
	
		if(!addFaces(collapsedFaces) ) {
			std::cout << "\n when add collapsed faces";
			PrintAddFaceWarning(collapsedFaces, false);
			return -1;
		}
/// no connection to fine faces
		if(!addFaces(reducedFaces, m_mesh->numVertices() - 1 ) ) {
			std::cout << "\n when add reduced faces";
			PrintAddFaceWarning(reducedFaces, false);
			return -1;
		}

		if(!addFaces(lastFaces) ) {
			std::cout << "\n when add last faces";
			PrintAddFaceWarning(lastFaces, false);
			return -1;
		}

		relocateVertices(vert2Remove, lastV, vaFaceInds, vbFaceInds);

		const int &ntri = m_mesh->numTriangles();
/// to the very end
		relocateFacesTo(vaFaceInds, ntri - 1);

		const int nfRemove = reducedFaces.size();

/// to the end but before faces to be reduced
		insertFacesAt(collapsedFaces, ntri - nfRemove);

/// hide last vertex and fine faces
		m_mesh->removeLastVertices(1);
		m_mesh->removeLastFaces(nfRemove);

		numCoarseFaces += collapsedFaces.size();
		numFineFaces += nfRemove;

		updateCost(collapsedFaces);
		lockVertices(collapsedFaces);
		updateCost(lastFaces);
		
		if(!checkTopology() ) return -1;

	}
	return nvStageBegin - m_mesh->numVertices();
}

void EdgeCollapse::buildTopology()
{
	const Vector3F *pos = m_mesh->c_positions();

	m_numFaces = m_mesh->numTriangles();
	m_numVertices = m_mesh->numVertices();
	m_vertices = new VertexValue[m_numVertices];

	const unsigned *ind = m_mesh->c_indices();
	for(int i=0;i<m_mesh->numIndices();i+=3) {
		m_edges[EdgeIndex(ind[i], ind[i + 1])] = EdgeValue();
		m_edges[EdgeIndex(ind[i + 1], ind[i + 2])] = EdgeValue();
		m_edges[EdgeIndex(ind[i + 2], ind[i])] = EdgeValue();
	}

	bool stat;
	for(int i=0;i<m_mesh->numIndices();i+=3) {
		const int j = i/3;

		FaceIndex fi(ind[i], ind[i + 1], ind[i+2]);
		m_tris[fi] = FaceValue(j);
		m_tris[fi].calculateArea(pos, fi);

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

	std::cout<<"\n n vert "<<m_numVertices
		<<" n face "<<m_tris.size()
		<<" n edge "<<m_edges.size();
}

void EdgeCollapse::computeEdgeCost()
{
	m_numBorderVertices = 0;
	for(int i=0;i<m_mesh->numVertices();++i) {
		if(isVertexOnBorder(i, m_vertices[i]))
			m_numBorderVertices++;
		else
			computeVertexCost(m_vertices[i], i);
	}

	std::map<EdgeIndex, EdgeValue>::iterator it = m_edges.begin();
	for(;it!=m_edges.end();++it) {
		if(!it->second.isOnBorder()) {
			computeEdgeCost(it->second, it->first);
		}
	}
	std::cout << " nv on border " << m_numBorderVertices;
}

EdgeIndex EdgeCollapse::findEdgeToCollapse()
{
	EdgeIndex collapseEdgeI;
	float edgeCostMin = 1e28f;
	
	std::map<EdgeIndex, EdgeValue>::iterator it = m_edges.begin();
	for(;it!=m_edges.end();++it) {
		const EdgeIndex &ei = it->first;
		if(!canEdgeCollapse(ei)) continue;

		if(edgeCostMin > it->second.cost()) {
			edgeCostMin = it->second.cost();
			collapseEdgeI = it->first;
		}
	}
	//std::cout<<"\n collapse edge "<<collapseEdgeI
	//	<<" cost "<<edgeCostMin;
	
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
	// std::cout<<" remove vert "<<a;
}

bool EdgeCollapse::isVertexOnBorder(int vi, const VertexValue &vert)
{ 
    if(vert.connectedFaces().size() < 3) return true;
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

		//std::cout<<"\n remove face "<<fi;
		m_tris.erase(ff);
/// disconnect valence vertices
		if(fi.v0() != vi)
			m_vertices[fi.v0()].disconnectFace(fi);

		if(fi.v1() != vi)
			m_vertices[fi.v1()].disconnectFace(fi);

		if(fi.v2() != vi)
			m_vertices[fi.v2()].disconnectFace(fi);
		
/// disconnect valence edges
        EdgeIndex ei0(fi.v0(), fi.v1());
        if(!ei0.hasV(vi)) {
            m_edges[ei0].disconnectFace(fi);
        }
        
        EdgeIndex ei1(fi.v1(), fi.v2());
        if(!ei1.hasV(vi)) {
            m_edges[ei1].disconnectFace(fi);
        }
        
        EdgeIndex ei2(fi.v2(), fi.v0());
        if(!ei2.hasV(vi)) {
            m_edges[ei2].disconnectFace(fi);
        }
	}
	
	return true;
}

void EdgeCollapse::removeEdge(const EdgeIndex &ei)
{
	std::map<EdgeIndex, EdgeValue>::iterator fe = m_edges.find(ei);
	if(fe != m_edges.end())
		m_edges.erase(fe);
}

bool EdgeCollapse::addFaces(const std::deque<FaceIndex> &faces, int lastV)
{
    bool stat;
	std::deque<FaceIndex>::const_iterator it = faces.begin();
	for(;it!=faces.end();++it) {
		const FaceIndex &fi = *it;
		//std::cout<<"\n add face "<<fi;
/// skip connection
		const bool isInRange = fi.v2() < lastV;
		
		EdgeIndex e0(fi.v0(), fi.v1());
		addEdge(e0);
		if(isInRange) {
		    stat = m_edges[e0].connectToFace(fi);
		    PrintUnmanifoldEdgeWarning(fi, m_edges[e0], stat);
		    if(!stat) return false;
		}
		
		EdgeIndex e1(fi.v1(), fi.v2());
		addEdge(e1);
		if(isInRange) {
		    stat = m_edges[e1].connectToFace(fi);
		    PrintUnmanifoldEdgeWarning(fi, m_edges[e1], stat);
		    if(!stat) return false;
		}
		
		EdgeIndex e2(fi.v2(), fi.v0());
		addEdge(e2);
		if(isInRange) {
		    stat = m_edges[e2].connectToFace(fi);
		    PrintUnmanifoldEdgeWarning(fi, m_edges[e2], stat);
		    if(!stat) return false;
		}
		
		m_tris[fi] = FaceValue();

		if(isInRange) {
		    m_vertices[fi.v0()].connectToFace(fi);
		    m_vertices[fi.v1()].connectToFace(fi);
		    m_vertices[fi.v2()].connectToFace(fi);
		}
	}
	return true;
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

bool EdgeCollapse::lastConnectedFaceOor(const VertexValue &vert)
{
    const int nf = vert.connectedFaces().size();
    std::deque<FaceIndex>::const_iterator it = vert.connectedFaces().begin();
	for(;it!=vert.connectedFaces().end();++it) {
		const FaceIndex &fi = *it;

		if(m_tris.find(fi) == m_tris.end()) {
			std::cout << "\n\n ERROR nonexistent face " << fi;
			return true;
		}

		if(m_tris[fi].ind() + nf > m_mesh->numTriangles() - 2)
		    return true;
	}
	return false;
}

void EdgeCollapse::relocateVertices(int va, int vb,
                const std::vector<int> &vaFaces,
                const std::vector<int> &vbFaces)
{
	bool isVbLocked = m_vertices[vb].isLocked();
	m_mesh->swapVertex(va, vb,
				vaFaces, vbFaces);
	m_vertices[va].lock();
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
	const Vector3F *pos = m_mesh->c_positions();

	std::deque<FaceIndex>::const_iterator it = faces.begin();

	for(;it!=faces.end();++it) {
		const FaceIndex &fi = *it;
		m_tris[fi].calculateArea(pos, fi);
	}

	it = faces.begin();
	for(;it!=faces.end();++it) {
		const FaceIndex &fi = *it;

		computeVertexCost(m_vertices[fi.v0()], fi.v0());
		computeVertexCost(m_vertices[fi.v1()], fi.v1());
		computeVertexCost(m_vertices[fi.v2()], fi.v2());
	}

	it = faces.begin();
	for(;it!=faces.end();++it) {
		const FaceIndex &fi = *it;
		const EdgeIndex e0(fi.v0(), fi.v1());
		if(!m_edges[e0].isOnBorder()) {
			computeEdgeCost(m_edges[e0], e0);
		}
		const EdgeIndex e1(fi.v1(), fi.v2());
		if(!m_edges[e1].isOnBorder()) {
			computeEdgeCost(m_edges[e1], e1);
		}
		const EdgeIndex e2(fi.v2(), fi.v0());
		if(!m_edges[e2].isOnBorder()) {
			computeEdgeCost(m_edges[e2], e2);
		}
	}
}

void EdgeCollapse::lockVertices(const std::deque<FaceIndex> &faces)
{
    std::deque<FaceIndex>::const_iterator it = faces.begin();
	for(;it!=faces.end();++it) {
		const FaceIndex &fi = *it;

		m_vertices[fi.v0()].lock();
		m_vertices[fi.v1()].lock();
		m_vertices[fi.v2()].lock();
	}
}

void EdgeCollapse::unlockAllVertices()
{
	for(int i=0;i<m_mesh->numVertices();++i)
		m_vertices[i].unlock();
}

void EdgeCollapse::relocateFacesTo(const std::vector<int> &faces, int toLastFace)
{
	int toFace = toLastFace;
	std::vector<int>::const_iterator it = faces.begin();

    for(;it!=faces.end();++it) {
    	//std::cout<<"\n swap f " << *it << "  "<<toFace;

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
    	m_mesh->swapHistory(*it, toFace);
    	m_mesh->setHistory(toFace);
    	
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

		//std::cout<<" insert " << fi;
	}

	m_mesh->insertFaces(cfv, location);
	m_mesh->insertHistory(faces.size(), location);

	for(int i=location;i<m_mesh->numTriangles();++i) {
		const unsigned *fv = &m_mesh->c_indices()[i * 3];
		const FaceIndex fi(fv[0], fv[1], fv[2]);

		if(m_tris.find(fi) == m_tris.end() ) {
			std::cout << "\n\n WARNING nonexistent face" <<fi;
		} else {
			m_tris[fi].ind() = i;
			//std::cout<< "\n f "<<i<<fi;
		}
	}
}

void EdgeCollapse::computeVertexCost(VertexValue &vert, int vi)
{
	const Vector3F *nmls = m_mesh->c_normals();
	const Vector3F &vn = nmls[vi];

	float totalArea = 0.f;
	vert.cost() = 0.f;
	std::deque<FaceIndex>::const_iterator it = vert.connectedFaces().begin();
	for(;it != vert.connectedFaces().end(); ++it) {
		const FaceIndex &fi = *it;

		if(fi.v0() != vi) 
			vert.higherCost(vn, nmls[fi.v0()]);

		if(fi.v1() != vi)
			vert.higherCost(vn, nmls[fi.v1()]);
		
		if(fi.v2() != vi)
			vert.higherCost(vn, nmls[fi.v2()]);

		totalArea += m_tris[fi].area();
	}
	vert.cost() *= totalArea;
}

void EdgeCollapse::computeEdgeCost(EdgeValue &e, const EdgeIndex &ei) const
{
	const Vector3F *pos = m_mesh->c_positions();
	float c = m_vertices[ei.v0()].cost();
	float cb = m_vertices[ei.v1()].cost();
	if(c < cb) c = cb;

	e.cost() = pos[ei.v0()].distanceTo(pos[ei.v1()]) * c;
}

bool EdgeCollapse::canEdgeCollapse(const EdgeIndex &ei)
{
	if(m_edges[ei].isOnBorder()) return false;
	
	const VertexValue &va = m_vertices[ei.v0()];
	const VertexValue &vb = m_vertices[ei.v1()];
	
	if(va.isLocked() || vb.isLocked())
		return false;

	if(va.connectedFaces().size() > 9 
			|| vb.connectedFaces().size() > 9 )
		return false;

	int vci = m_edges[ei].face0().getOpposite(ei.v0(), ei.v1());
	int vdi = m_edges[ei].face1().getOpposite(ei.v0(), ei.v1());
	
	if(m_vertices[vci].connectedFaces().size() < 4 
			|| m_vertices[vdi].connectedFaces().size() < 4 )
		return false;

	int lastVert = m_mesh->numVertices() - 2;
	
	if(va.lastConnectedVertex() > lastVert 
			|| vb.lastConnectedVertex() > lastVert)
		return false;
/// limite range of connected face for relocate
    if(lastConnectedFaceOor(va))
        return false;
    if(lastConnectedFaceOor(vb))
        return false;

	if(isVertexOnBorder(ei.v0(), va)
			|| isVertexOnBorder(ei.v1(), vb))
		return false;

	return true;
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
	//std::cout<< "\n passed checkTopology ";
	return true;
}

bool EdgeCollapse::canEndProcess() const
{ 
	return m_mesh->numVertices() < (m_numBorderVertices + (m_numVertices >> 3) ); 
}

void EdgeCollapse::PrintUnmanifoldEdgeWarning(const FaceIndex &fi, const EdgeValue &e,
                bool stat)
{ 
    if(!stat) 
        std::cout << "\n\n WARNING avoid unmanifold edge " << e << " face " << fi; 
}

void EdgeCollapse::PrintAddFaceWarning(const std::deque<FaceIndex> &faces, 
                bool stat)
{
	if(stat) return;
	std::cout << "\n\n WARNING cannot add faces [";
	for(int i=0;i<faces.size();++i)
		std::cout << " " << faces[i];
	std::cout << "] ";
}

void EdgeCollapse::PrintCollapseEdgeError(int va, int vb,
	const VertexValue &a,
			const VertexValue &b) 
{
	std::cout<<"\n\n ERROR collapsing edge from v" << va << a
		<<"\n to v" << vb << b;
}

}
