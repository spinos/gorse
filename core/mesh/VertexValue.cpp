#include "VertexValue.h"
#include <math/Vector3F.h>
#include <algorithm>

namespace alo {

VertexValue::VertexValue() : m_cost(1e28f),
m_locked(false),
m_onborder(false)
{}
	
VertexValue::~VertexValue()
{ 
	m_faceInds.clear(); 
	m_pastFaceInds.clear();
}

void VertexValue::connectToFace(const FaceIndex &x)
{ 
	if(m_faceInds.size() < 1) {
		m_faceInds.push_back(x); 
		return;
	}

	if(m_faceInds.front() > x) {
		m_faceInds.push_front(x); 
		return;
	}

	if(m_faceInds.back() < x) {
		m_faceInds.push_back(x); 
		return;
	}

	std::deque<FaceIndex>::iterator it = m_faceInds.begin();
	for(;it!=m_faceInds.end();++it) {
		if(*it == x) return;
		if(*it > x) {
			m_faceInds.insert(it, x);
			return;
		}
	}
}

void VertexValue::connectToPastFace(const FaceIndex &x)
{ 
	if(m_pastFaceInds.size() < 1) {
		m_pastFaceInds.push_back(x); 
		return;
	}

	if(m_pastFaceInds.front() > x) {
		m_pastFaceInds.push_front(x); 
		return;
	}

	if(m_pastFaceInds.back() < x) {
		m_pastFaceInds.push_back(x); 
		return;
	}

	std::deque<FaceIndex>::iterator it = m_pastFaceInds.begin();
	for(;it!=m_pastFaceInds.end();++it) {
		if(*it == x) return;
		if(*it > x) {
			m_pastFaceInds.insert(it, x);
			return;
		}
	}
}

void VertexValue::disconnectFace(const FaceIndex &x)
{
	std::deque<FaceIndex>::iterator it = m_faceInds.begin();
	for(;it!=m_faceInds.end();++it) {

		if(*it == x)  {
			m_faceInds.erase(it);
			return;
		}

		if(*it > x) return;
	}
}

void VertexValue::disconnectPastFace(const FaceIndex &x)
{
	std::deque<FaceIndex>::iterator it = m_pastFaceInds.begin();
	for(;it!=m_pastFaceInds.end();++it) {

		if(*it == x)  {
			m_pastFaceInds.erase(it);
			return;
		}

		if(*it > x) return;
	}
}

void VertexValue::clearFaces()
{ m_faceInds.clear(); }

void VertexValue::clearPastFaces()
{ m_pastFaceInds.clear(); }

float &VertexValue::cost()
{ return m_cost; }

const float &VertexValue::cost() const
{ return m_cost; }

void VertexValue::setOnBorder(bool x)
{ m_onborder = x; }

const bool &VertexValue::isOnBorder() const
{ return m_onborder; }

void VertexValue::getConnectedVertices(std::deque<int> &vs,
			int vi) const
{
	std::deque<FaceIndex>::const_iterator it = m_faceInds.begin();
	for(;it!=m_faceInds.end();++it) {
		const FaceIndex &fi = *it;

		if(fi.v0() != vi) 
			addToVector(vs, fi.v0());

		if(fi.v1() != vi)
			addToVector(vs, fi.v1());
		
		if(fi.v2() != vi)
			addToVector(vs, fi.v2());
	}
}

void VertexValue::getReformedFaces(std::deque<FaceIndex> &faces, int vi, int vb) const
{
	std::deque<FaceIndex>::const_iterator it = m_faceInds.begin();
	for(;it!=m_faceInds.end();++it) {
		const FaceIndex &fi = *it;
		FaceIndex fr = fi.reformed(vi, vb);
		faces.push_back(fr);
	}
}

void VertexValue::addToVector(std::deque<int> &vs, int x) const
{
	if(std::find(vs.begin(), vs.end(), x) == vs.end()) 
		vs.push_back(x);
}

bool VertexValue::checkFaces(int vi) const
{
	std::deque<FaceIndex>::const_iterator it = m_faceInds.begin();
	for(;it!=m_faceInds.end();++it) {
		const FaceIndex &fi = *it;
		if(!fi.hasV(vi) ) {
			std::cout<<"\n\n ERROR face"<<fi<< " not connected to vert "<<vi;
			return false;
		}
	}
	return true;
}

void VertexValue::lock()
{ m_locked = true; }

void VertexValue::unlock()
{ m_locked = false; }

const bool &VertexValue::isLocked() const
{ return m_locked; }

bool VertexValue::getOneRing(std::vector<int> &vertInds, int vi,
		const Vector3F *pos, const Vector3F &nv) const
{
	const int nf = m_faceInds.size();
	if(nf < 3) return false;
	int va, vb, vc;
	if(!m_faceInds[0].getOppositeEdge(va, vb, vi) )
		return false;

	const Vector3F &pv = pos[vi];
	const Vector3F nab = (pos[va] - pv).cross(pos[vb] - pv);
	if(nab.dot(nv) < 0.f) {
		vc = vb;
		vb = va;
		va = vc;
	}
	vertInds.push_back(va); 
	vertInds.push_back(vb);

	const int endv = va;
	for(;;) {
		if(!findNextVertex(vc, va, vb, vi))
			return false;

		if(vc == endv)
			break;

		vertInds.push_back(vc);

		va = vb;
		vb = vc;
	}

	return vertInds.size() == nf;
}

bool VertexValue::findNextVertex(int &c, const int &a, const int &b, const int &vi) const
{
	for(int i=0;i<m_faceInds.size();++i) {
		const FaceIndex &fi = m_faceInds[i];
		if(!fi.hasV(b)) continue;
		c = fi.getOpposite(b, vi);
		if(c > -1 && c != a)
			return true;
	}
	return false;
}

void VertexValue::copyPastFacesTo(std::vector<FaceIndex> &faces) const
{
	for(int i=0;i<m_pastFaceInds.size();++i) 
		faces.push_back(m_pastFaceInds[i]);
}

bool VertexValue::hasFaceConnected(const VertexValue &b) const
{
	std::deque<FaceIndex>::const_iterator it = b.facesBegin();
	for(;it!=b.facesEnd();++it) 
		if(hasFaceConnected(*it)) return true;
	
	return false;
}

bool VertexValue::hasFaceConnected(const FaceIndex &fi) const
{
	std::deque<FaceIndex>::const_iterator it = m_faceInds.begin();
	for(;it!=m_faceInds.end();++it) {
		if(it->isConnectedTo(fi)) return true;
		if(*it > fi) return false;
	}
	return false;
}

bool VertexValue::hasPastConnected(const VertexValue &b) const
{
	std::deque<FaceIndex>::const_iterator it = b.pastFacesBegin();
	for(;it!=b.pastFacesEnd();++it)
		if(hasPastConnected(*it)) return true;
	
	return false;
}

bool VertexValue::hasPastConnected(const FaceIndex &fi) const
{
	std::deque<FaceIndex>::const_iterator it = m_pastFaceInds.begin();
	for(;it!=m_pastFaceInds.end();++it) {
		if(it->isConnectedTo(fi)) return true;
		if(*it > fi) return false;
	}
	return false;
}

std::deque<FaceIndex>::const_iterator VertexValue::facesBegin() const
{ return m_faceInds.begin(); }

std::deque<FaceIndex>::const_iterator VertexValue::facesEnd() const
{ return m_faceInds.end(); }

std::deque<FaceIndex>::const_iterator VertexValue::pastFacesBegin() const
{ return m_pastFaceInds.begin(); }

std::deque<FaceIndex>::const_iterator VertexValue::pastFacesEnd() const
{ return m_pastFaceInds.end(); }

int VertexValue::numConnectedFaces() const
{ return m_faceInds.size(); }

int VertexValue::numConnectedPastFaces() const
{ return m_pastFaceInds.size(); }

std::ostream& operator<<(std::ostream &output, const VertexValue & p)
{
	output << " f [";
	std::deque<FaceIndex>::const_iterator it = p.facesBegin();
	for(;it!=p.facesEnd();++it) {
    	output << " " << *it;
    }
    output << "] past_f [";
    it = p.pastFacesBegin();
	for(;it!=p.pastFacesEnd();++it) {
    	output << " " << *it;
    }
    output << "] ";
    return output;
}

}
