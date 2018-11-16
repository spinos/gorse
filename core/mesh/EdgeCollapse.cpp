#include "EdgeCollapse.h"
#include "EdgeIndex.h"
#include "EdgeValue.h"
#include "FaceIndex.h"
#include "FaceValue.h"
#include "VertexValue.h"
#include "PolygonUVTriangulation.h"
#include "HistoryMesh.h"

namespace alo {

EdgeCollapse::EdgeCollapse()
{ m_triangulate = new PolygonUVTriangulation; }

EdgeCollapse::~EdgeCollapse()
{ delete m_triangulate; }

void EdgeCollapse::simplify(HistoryMesh *msh)
{
	m_mesh = msh;
	m_mesh->initUV();
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

	m_mesh->finishUV();
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

		int vertRed = getRedVertex(collapseEdgeI);
		const int vertGreen = m_mesh->numVertices() - 1;

/// red -----> 
///    <----- green
		const VertexValue &vred = vertex(vertRed);
		const VertexValue &vgreen = vertex(vertGreen);

		if(!getTriangulatePolygon(vertRed)) return -1;
		
/// vertice around red
		const std::vector<int> redRing(m_triangulate->vertices());
		
/// coarse faces to be created
		std::deque<FaceIndex> blueFaces;
		if(!m_triangulate->getTriangles(blueFaces)) {
			std::cout << "\n\n ERROR cannot triangulate";
			PrintOneRing(vertRed, redRing);
			return -1;
		}

		mapConnectedFaces(vred);
		
		std::deque<FaceValue> blueUVs;
		m_triangulate->getTriangleFaces(blueUVs, blueFaces);

		std::deque<FaceIndex> redFaces;
		vred.getFaces(redFaces);

/// connect red faces to green vertex
		std::deque<FaceValue> redUVs;
		reformFaces(redFaces, redUVs, vertRed, vertGreen);

		std::deque<FaceIndex> greenFaces;
		vgreen.getFaces(greenFaces);

/// connect green faces to red vertex
		std::deque<FaceValue> greenUVs;
		reformFaces(greenFaces, greenUVs, vertGreen, vertRed);

		std::vector<int> redFaceInds;
		getFaceInds(redFaceInds, redUVs);
		std::vector<int> greenFaceInds;
		getFaceInds(greenFaceInds, greenUVs);

/// to relocate past face connections 
		std::vector<FaceIndex> redPast;
		vred.copyPastFacesTo(redPast);
		vertex(vertRed).clearPastFaces();

		std::vector<FaceIndex> greenPast;
		vgreen.copyPastFacesTo(greenPast);
		vertex(vertGreen).clearPastFaces();

/// remove faces connected to red 
		if(!removeVertexConnection(vertRed)) {
			PrintCollapseEdgeError(vertRed, vertGreen,
				vred, vgreen);
			return -1;
		}

/// remove faces connected to green
		if(!removeVertexConnection(vertGreen)) {
			PrintCollapseEdgeError(vertRed, vertGreen,
				vred, vgreen);
			return -1;
		}
	
		if(!addFaces(blueFaces, blueUVs) ) {
			std::cout << "\n when add blue faces";
			PrintAddFaceWarning(blueFaces, false);
			PrintOneRing(vertRed, redRing);
			return -1;
		}
/// no connection to fine faces
		if(!addFaces(redFaces, redUVs, vertGreen ) ) {
			std::cout << "\n when add red faces";
			PrintAddFaceWarning(redFaces, false);
			return -1;
		}

		if(!addFaces(greenFaces, greenUVs) ) {
			std::cout << "\n when add green faces";
			PrintAddFaceWarning(greenFaces, false);
			return -1;
		}

		relocateVertices(vertRed, vertGreen, redFaceInds, greenFaceInds);

		const int &ntri = m_mesh->numTriangles();
/// red to the very end
		relocateFacesTo(redFaceInds, ntri - 1);

		const int nfRemove = redFaces.size();

/// blue to just before red
		insertFacesAt(blueFaces, blueUVs, ntri - nfRemove);
		indexPastFaces(m_mesh, ntri - nfRemove, ntri);

		std::vector<FaceIndex>::const_iterator pit;
		pit = redPast.begin();
		for(;pit!=redPast.end();++pit) 
			replaceMeshVertex(m_mesh, *pit, vertRed, vertGreen);
				
		redPast.clear();

		pit = greenPast.begin();
		for(;pit!=greenPast.end();++pit) 
			replaceMeshVertex(m_mesh, *pit, vertGreen, vertRed);
				
		greenPast.clear();

/// hide green vertex and red faces
		m_mesh->removeLastVertices(1);
		m_mesh->removeLastFaces(nfRemove);

		numCoarseFaces += blueFaces.size();
		numFineFaces += nfRemove;

		updateCost(blueFaces, redRing);
		lockVertices(redRing);
		updateCost(greenFaces);
#if 0		
		if(!checkTopology(m_mesh) ) return -1;
#endif
	}
	return nvStageBegin - m_mesh->numVertices();
}

void EdgeCollapse::computeEdgeCost()
{
	for(int i=0;i<m_mesh->numVertices();++i) {
		VertexValue &vi = vertex(i);
		if(!vi.isOnBorder())
			computeVertexCost(vi);
	}

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

	if(edgeCostMin > .9e28f) 
	    return EdgeIndex();
	
	return collapseEdgeI;
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
	updateFaces(faces);

	std::deque<FaceIndex>::const_iterator it = faces.begin();
	for(;it!=faces.end();++it) {
		const FaceIndex &fi = *it;

		computeVertexCost(vertex(fi.v0()));
		computeVertexCost(vertex(fi.v1()));
		computeVertexCost(vertex(fi.v2()));
	}

	computeEdgeCost(faces);
}

void EdgeCollapse::updateCost(const std::deque<FaceIndex> &faces,
						const std::vector<int> &vertices)
{
	updateFaces(faces);

	std::vector<int>::const_iterator vit = vertices.begin();
	for(;vit!=vertices.end();++vit)
		computeVertexCost(vertex(*vit));

	computeEdgeCost(faces);
}

void EdgeCollapse::updateFaces(const std::deque<FaceIndex> &faces)
{
	std::deque<FaceIndex>::const_iterator it = faces.begin();
	for(;it!=faces.end();++it) {
		const FaceIndex &fi = *it;
		FaceValue &facei = face(fi);
		const int &ti = facei.ind();
		facei.setArea(m_mesh->getTriangleArea(ti));
		facei.setNormal(m_mesh->getTriangleNormal(ti));
	}
}

void EdgeCollapse::computeEdgeCost(const std::deque<FaceIndex> &faces)
{
	std::deque<FaceIndex>::const_iterator it = faces.begin();
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

    	const Int3 &afv = m_mesh->c_indices()[*it];
    	FaceIndex afi(afv.x, afv.y, afv.z);

    	setFaceInd(afi, toFace);
   	
    	const Int3 &bfv = m_mesh->c_indices()[toFace];

    	FaceIndex bfi(bfv.x, bfv.y, bfv.z);

    	setFaceInd(bfi, *it);

    	m_mesh->swapFace(*it, toFace);
    	m_mesh->swapHistory(*it, toFace);
    	m_mesh->setHistory(toFace);
    	m_mesh->swapUV(*it, toFace);
    	
/// move forward
    	toFace--;
    }
}

void EdgeCollapse::insertFacesAt(const std::deque<FaceIndex> &faces, 
                        const std::deque<FaceValue> &uvs, 
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
	m_mesh->insertUV(cfv, uvs, location);
	m_mesh->insertHistory(faces.size(), location);

	for(int i=location;i<location + faces.size();++i) {
		const Int3 &fv = m_mesh->c_indices()[i];
		const FaceIndex fi(fv.x, fv.y, fv.z);
		if(!setFaceInd(fi, i) )
            std::cout<<"\n WARNING set ind to nonexistent face "<<fi
                    <<" insert "<<i<<" location "<<location;
	}
    
    pushPastFaceIndex(faces.size());
}

void EdgeCollapse::computeVertexCost(VertexValue &vert)
{
	const int faceCount = vert.numConnectedFaces();
	std::pair<Vector3F, float> * faceCost = new std::pair<Vector3F, float>[faceCount];

	float totalArea = 0.f;
	int i=0;
	std::deque<FaceIndex>::const_iterator it = vert.facesBegin();
	for(;it != vert.facesEnd(); ++it) {
		const FaceIndex &fi = *it;
		const FaceValue &facei = face(fi);

		faceCost[i].first = facei.normal();
		i++;

		totalArea += facei.area();
	}

	for(i=0;i<faceCount;++i) {
		faceCost[i].second = 0.f;
		const Vector3F &ni = faceCost[i].first;
		for(int j=0;j<faceCount;++j) {
			if(j==i) continue;
			float nidnj = 1.f - ni.dot(faceCost[j].first);
			if(faceCost[i].second < nidnj)
				faceCost[i].second = nidnj;
		}
	}

	vert.cost() = 0.f;
	for(i=0;i<faceCount;++i) {
		if(vert.cost() < faceCost[i].second)
			vert.cost() = faceCost[i].second;
	}

	delete[] faceCost;
	
/// not flat enough
	if(vert.cost() > .3573f) /// 50 deg
		vert.cost() = 1e29f;
	else 
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

	if(va.numConnectedFaces() < 4 
			|| vb.numConnectedFaces() < 4 )
		return false;

	if(va.numConnectedFaces() > 10 
			|| vb.numConnectedFaces() > 10 )
		return false;

	int vci = e.face0().getOpposite(ei.v0(), ei.v1());
	int vdi = e.face1().getOpposite(ei.v0(), ei.v1());
	
	if(vertex(vci).numConnectedFaces() < 4 
			|| vertex(vdi).numConnectedFaces() < 4 )
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
	return m_mesh->numVertices() < (numBorderVertices() + (numVertices() >> 4) ); 
}

void EdgeCollapse::mapConnectedFaces(const VertexValue &v)
{
	m_triangulate->clearUVLookup();
	std::deque<FaceIndex>::const_iterator it = v.facesBegin();
	for(;it!=v.facesEnd();++it) 
		m_triangulate->mapFace(*it, face(*it));
}

bool EdgeCollapse::getTriangulatePolygon(int i)
{
	m_triangulate->setCenter(m_mesh->c_positions()[i]);

	std::vector<int> &vring = m_triangulate->vertices();
	vring.clear();
	const VertexValue &vred = vertex(i);
	if(!vred.getOneRing(vring, i,
		m_mesh->c_positions(),
		m_mesh->c_normals()[i])) {
		std::cout << "\n\n ERROR not one ring v" << i << vred;
		return false;
	}
	return true;
}

int EdgeCollapse::getRedVertex(const EdgeIndex &ei) const
{
/// lower cost
	if(vertex(ei.v0()).cost() < vertex(ei.v1()).cost()) 
		return ei.v0();

	return ei.v1();
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

void EdgeCollapse::PrintOneRing(int va, const std::vector<int> &ring)
{
	std::cout<<"\n\n INFO one ring around vertex " << va <<" [";

	std::vector<int>::const_iterator it = ring.begin();
	for(;it!=ring.end();++it)
		std::cout<<" "<<*it;

	std::cout<<"] ";
}

}
