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
{ delete m_triangulate; }

void EdgeCollapse::simplify(HistoryMesh *msh)
{
	m_mesh = msh;
	m_mesh->initHistory();

	m_triangulate->setPositions(m_mesh->c_positions());
	
	buildTopology(m_mesh);

	computeEdgeCost();

	for(int istage = 0;istage < m_mesh->maxNumStages();++istage) {
		m_mesh->addHistoryStage();
		int ncoarse = 0, nfine = 0;
		int dnf = processStage(ncoarse, nfine);
		m_mesh->finishHistoryStage(ncoarse, nfine);
		if(dnf < 1 || canEndProcess()) break;
		unlockVertices(m_mesh->numVertices());
	}

	m_mesh->finishHistory(numVertices(), numFaces());
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

		const VertexValue &vrm = vertex(vert2Remove);
		
		std::vector<int> &vring = m_triangulate->vertices();
		vring.clear();
		if(!vrm.getOneRing(vring, vert2Remove,
			m_mesh->c_positions(),
			m_mesh->c_normals()[vert2Remove])) {

			std::cout << "\n\n ERROR not one ring v" << vert2Remove << vrm;
			return -1;
		}

/// coarse faces to be created
		std::deque<FaceIndex> coarseFaces;
		if(!m_triangulate->getTriangles(coarseFaces)) {
			std::cout << "\n\n ERROR wrong triangulate n face" << coarseFaces.size();
			return -1;
		}

		std::deque<FaceIndex> lastFaces;
		std::deque<FaceIndex> fineFaces;
		std::vector<int> vaFaceInds;
		std::vector<int> vbFaceInds;
/// relocated
		vrm.getReformedFaces(fineFaces, vert2Remove, lastV);

/// to be connected to relocated vertex
		getConnectedFaceInds(vaFaceInds, vrm);
		getConnectedFaceInds(vbFaceInds, vertex(lastV));
		vertex(lastV).getReformedFaces(lastFaces, lastV, vert2Remove);

/// to relocate past face connections 
		std::vector<FaceIndex> aPast;
		vertex(vert2Remove).copyPastFacesTo(aPast);
		vertex(vert2Remove).clearPastFaces();

		std::vector<FaceIndex> bPast;
		vertex(lastV).copyPastFacesTo(bPast);
		vertex(lastV).clearPastFaces();

/// remove faces connected to vertex 
		if(!removeVertexConnection(vert2Remove)) {
			PrintCollapseEdgeError(vert2Remove, lastV,
				vertex(vert2Remove), vertex(lastV));
			return -1;
		}

/// remove faces connected to last vertex
		if(!removeVertexConnection(lastV)) {
			PrintCollapseEdgeError(vert2Remove, lastV,
				vertex(vert2Remove), vertex(lastV));
			return -1;
		}
	
		if(!addFaces(coarseFaces) ) {
			std::cout << "\n when add collapsed faces";
			PrintAddFaceWarning(coarseFaces, false);
			return -1;
		}
/// no connection to fine faces
		if(!addFaces(fineFaces, lastV ) ) {
			std::cout << "\n when add reduced faces";
			PrintAddFaceWarning(fineFaces, false);
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

		const int nfRemove = fineFaces.size();

/// to the end but before faces to be reduced
		insertFacesAt(coarseFaces, ntri - nfRemove);
		indexPastFaces(m_mesh, ntri - nfRemove, ntri);

		std::vector<FaceIndex>::const_iterator pit;
		pit = aPast.begin();
		for(;pit!=aPast.end();++pit) 
			replaceMeshVertex(m_mesh, *pit, vert2Remove, lastV);
				
		aPast.clear();

		pit = bPast.begin();
		for(;pit!=bPast.end();++pit) 
			replaceMeshVertex(m_mesh, *pit, lastV, vert2Remove);
				
		bPast.clear();

/// hide last vertex and fine faces
		m_mesh->removeLastVertices(1);
		m_mesh->removeLastFaces(nfRemove);

		numCoarseFaces += coarseFaces.size();
		numFineFaces += nfRemove;

		updateCost(coarseFaces);
		lockFaces(coarseFaces);
		updateCost(lastFaces);
		
		if(!checkTopology(m_mesh) ) return -1;

	}
	return nvStageBegin - m_mesh->numVertices();
}

void EdgeCollapse::computeEdgeCost()
{
	int nbv = 0;
	for(int i=0;i<m_mesh->numVertices();++i) {
		VertexValue &vi = vertex(i);
		if(isVertexOnBorder(i, vi)) {
/// always on border
			vi.setOnBorder(true);
			nbv++;
		} else {
			computeVertexCost(vi, i);
			vi.setOnBorder(false);
		}
	}

	setNumBorderVertices(nbv);

	std::map<EdgeIndex, EdgeValue>::iterator it = edgesBegin();
	for(;it!=edgesEnd();++it) {
		if(!it->second.isOnBorder()) {
			computeEdgeCost(it->second, it->first);
		}
	}

}

EdgeIndex EdgeCollapse::findEdgeToCollapse()
{
	EdgeIndex collapseEdgeI;
	float edgeCostMin = 1e28f;
	
	std::map<EdgeIndex, EdgeValue>::iterator it = edgesBegin();
	for(;it!=edgesEnd();++it) {
		const EdgeIndex &ei = it->first;
		if(edgeCostMin <= it->second.cost()) continue;

		if(!canEdgeCollapse(ei)) continue;

		edgeCostMin = it->second.cost();
		collapseEdgeI = it->first;

	}
	//std::cout<<"\n collapse edge "<<collapseEdgeI
	//	<<" cost "<<edgeCostMin;
	
	return collapseEdgeI;
}

void EdgeCollapse::getVertexToRemove(int &a, int &b, const EdgeIndex &ei)
{
	a = ei.v0();
	b = ei.v1();
/// lower cost
	if(vertex(a).cost() > vertex(b).cost()) {
		int c = a;
		a = b;
		b = c;
	}
}

bool EdgeCollapse::lastConnectedFaceOor(const VertexValue &vert)
{
    const int nf = vert.numConnectedFaces();
    std::deque<FaceIndex>::const_iterator it = vert.facesBegin();
	for(;it!=vert.facesEnd();++it) {
		const FaceIndex &fi = *it;

		if(!faceExists(fi)) continue;

/// cannot connect to face to be relocated
		if(face(fi).ind() + nf > m_mesh->numTriangles() - 2)
		    return true;
	}
	return false;
}

void EdgeCollapse::relocateVertices(int va, int vb,
                const std::vector<int> &vaFaces,
                const std::vector<int> &vbFaces)
{
	bool aonborder = vertex(va).isOnBorder();
	bool bonborder = vertex(vb).isOnBorder();
	m_mesh->swapVertex(va, vb,
				vaFaces, vbFaces);
	vertex(va).lock();
	vertex(va).setOnBorder(bonborder);
	vertex(vb).setOnBorder(aonborder);
	
/// re-index affected faces
	std::vector<int>::const_iterator it = vaFaces.begin();
	for(;it!=vaFaces.end();++it)
		setMeshFaceInd(m_mesh, *it);

	it = vbFaces.begin();
	for(;it!=vbFaces.end();++it)
		setMeshFaceInd(m_mesh, *it);

}

void EdgeCollapse::updateCost(const std::deque<FaceIndex> &faces)
{
	const unsigned *ind = m_mesh->c_indices();
	const Vector3F *nmls = m_mesh->c_normals();
	const Vector3F *pos = m_mesh->c_positions();

	std::deque<FaceIndex>::const_iterator it = faces.begin();

	for(;it!=faces.end();++it) {
		const FaceIndex &fi = *it;
		face(fi).calculateArea(pos, fi);
	}

	it = faces.begin();
	for(;it!=faces.end();++it) {
		const FaceIndex &fi = *it;

		computeVertexCost(vertex(fi.v0()), fi.v0());
		computeVertexCost(vertex(fi.v1()), fi.v1());
		computeVertexCost(vertex(fi.v2()), fi.v2());
	}

	it = faces.begin();
	for(;it!=faces.end();++it) {
		const FaceIndex &fi = *it;
		const EdgeIndex e0(fi.v0(), fi.v1());
		if(!edge(e0).isOnBorder()) {
			computeEdgeCost(edge(e0), e0);
		}
		const EdgeIndex e1(fi.v1(), fi.v2());
		if(!edge(e1).isOnBorder()) {
			computeEdgeCost(edge(e1), e1);
		}
		const EdgeIndex e2(fi.v2(), fi.v0());
		if(!edge(e2).isOnBorder()) {
			computeEdgeCost(edge(e2), e2);
		}
	}
}

void EdgeCollapse::relocateFacesTo(const std::vector<int> &faces, int toLastFace)
{
	int toFace = toLastFace;
	std::vector<int>::const_iterator it = faces.begin();

    for(;it!=faces.end();++it) {

    	const unsigned *afv = &m_mesh->c_indices()[*it * 3];
    	int fv[3];
    	fv[0] = afv[0];
    	fv[1] = afv[1];
    	fv[2] = afv[2];
    	FaceIndex afi(fv[0], fv[1], fv[2]);

    	setFaceInd(afi, toFace);
   	
    	const unsigned *bfv = &m_mesh->c_indices()[toFace * 3];

    	fv[0] = bfv[0];
    	fv[1] = bfv[1];
    	fv[2] = bfv[2];
    	FaceIndex bfi(fv[0], fv[1], fv[2]);

    	setFaceInd(bfi, *it);

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

	}

	m_mesh->insertFaces(cfv, location);
	m_mesh->insertHistory(faces.size(), location);

	for(int i=location;i<location + faces.size();++i) {
		const unsigned *fv = &m_mesh->c_indices()[i * 3];
		const FaceIndex fi(fv[0], fv[1], fv[2]);
		if(!setFaceInd(fi, i) )
            std::cout<<"\n WARNING set ind to nonexistent face "<<fi
                    <<" insert "<<i<<" location "<<location;
	}
    
    pushPastFaceIndex(faces.size());
}

void EdgeCollapse::computeVertexCost(VertexValue &vert, int vi)
{
	const Vector3F *nmls = m_mesh->c_normals();
	const Vector3F &vn = nmls[vi];

	float totalArea = 0.f;
	vert.cost() = 0.f;
	std::deque<FaceIndex>::const_iterator it = vert.facesBegin();
	for(;it != vert.facesEnd(); ++it) {
		const FaceIndex &fi = *it;

		if(fi.v0() != vi) 
			vert.higherCost(vn, nmls[fi.v0()]);

		if(fi.v1() != vi)
			vert.higherCost(vn, nmls[fi.v1()]);
		
		if(fi.v2() != vi)
			vert.higherCost(vn, nmls[fi.v2()]);

		totalArea += face(fi).area();
	}
	vert.cost() *= totalArea;
}

void EdgeCollapse::computeEdgeCost(EdgeValue &e, const EdgeIndex &ei) const
{
	const Vector3F *pos = m_mesh->c_positions();
	float c = vertex(ei.v0()).cost();
	float cb = vertex(ei.v1()).cost();
	if(c < cb) c = cb;

	e.cost() = pos[ei.v0()].distanceTo(pos[ei.v1()]) * c;
}

bool EdgeCollapse::canEdgeCollapse(const EdgeIndex &ei)
{
	const EdgeValue &e = edge(ei);
	if(e.isOnBorder()) return false;
	
	const VertexValue &va = vertex(ei.v0());
	const VertexValue &vb = vertex(ei.v1());
	
	if(va.isLocked() || vb.isLocked())
		return false;

	if(va.isOnBorder() || vb.isOnBorder())
		return false;

	if(va.numConnectedFaces() > 9 
			|| vb.numConnectedFaces() > 9 )
		return false;

	int vci = e.face0().getOpposite(ei.v0(), ei.v1());
	int vdi = e.face1().getOpposite(ei.v0(), ei.v1());
	
	if(vertex(vci).numConnectedFaces() < 5 
			|| vertex(vdi).numConnectedFaces() < 5 )
		return false;

	int lastVert = m_mesh->numVertices() - 2;
	const VertexValue &vlast = vertex(m_mesh->numVertices() - 1);
    
/// cannot share faces with last v
/// present or past
	if(va.hasFaceConnected(vlast))
		return false; 
	if(vb.hasFaceConnected(vlast))
		return false;

    if(va.hasPastConnected(vlast))
    	return false;
    if(vb.hasPastConnected(vlast))
    	return false;

/// limite range of connected face for relocate
    if(lastConnectedFaceOor(va))
        return false;
    if(lastConnectedFaceOor(vb))
        return false;

	return true;
}

bool EdgeCollapse::canEndProcess() const
{ 
	return m_mesh->numVertices() < (numBorderVertices() + (numVertices() >> 3) ); 
}

void EdgeCollapse::PrintCollapseEdgeError(int va, int vb,
	const VertexValue &a,
			const VertexValue &b) 
{
	std::cout<<"\n\n ERROR collapsing edge from v" << va << a
		<<"\n to v" << vb << b;
}

void EdgeCollapse::PrintConnectedPastWarning(int va, int vb,
            const VertexValue &a,
            const VertexValue &b)
{
	std::cout<<"\n\n WARNING connected past " << va <<" " << vb
			<<"\n a past [";
	std::deque<FaceIndex>::const_iterator it = a.pastFacesBegin();
	for(;it!=a.pastFacesEnd();++it)
		std::cout<<" "<<*it;
	std::cout<<"]\n b past [";
	it = b.pastFacesBegin();
	for(;it!=b.pastFacesEnd();++it)
		std::cout<<" "<<*it;
	std::cout<<"] ";

}

}
