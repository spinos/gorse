/*
 *  VertexFaceConnection.cpp
 *  aloe
 *
 *
 */

#include "VertexFaceConnection.h"
#include <math/Vector3F.h>
#include <algorithm>

namespace alo {

VertexFaceConnection::VertexFaceConnection()
{}

VertexFaceConnection::~VertexFaceConnection()
{}

void VertexFaceConnection::resize(int n)
{ m_connections.resize(n); }

void VertexFaceConnection::clearFaces(int v)
{ m_connections[v].clear(); }

void VertexFaceConnection::connectFace(const FaceIndex &fi)
{
	connectFace(m_connections[fi.v0()], fi);
	connectFace(m_connections[fi.v1()], fi);
	connectFace(m_connections[fi.v2()], fi);
}

void VertexFaceConnection::disconnectFace(const FaceIndex &fi)
{
	disconnectFace(m_connections[fi.v0()], fi);
	disconnectFace(m_connections[fi.v1()], fi);
	disconnectFace(m_connections[fi.v2()], fi);
}

void VertexFaceConnection::disconnectFace(int v, const FaceIndex &fi)
{ disconnectFace(m_connections[v], fi); }

int VertexFaceConnection::numFacesConnected(int v) const
{ return m_connections[v].size(); }

void VertexFaceConnection::copyFacesTo(std::vector<FaceIndex> &faces, int v) const
{
	const std::deque<FaceIndex> &c = m_connections[v];
	for(int i=0;i<c.size();++i) 
		faces.push_back(c[i]);
}

void VertexFaceConnection::connectFace(std::deque<FaceIndex> &c, const FaceIndex &x)
{ 
	if(c.size() < 1) {
		c.push_back(x); 
		return;
	}

	if(c.front() > x) {
		c.push_front(x); 
		return;
	}

	if(c.back() < x) {
		c.push_back(x); 
		return;
	}

	std::deque<FaceIndex>::iterator it = c.begin();
	for(;it!=c.end();++it) {
		if(*it == x) return;
		if(*it > x) {
			c.insert(it, x);
			return;
		}
	}
}

void VertexFaceConnection::disconnectFace(std::deque<FaceIndex> &c, const FaceIndex &x)
{
	if(c.size() < 1) return;

	int low = 0;
	if(c[low] == x) {
		c.erase(c.begin() + low);
		return;
	}
	int high = c.size() - 1;
	if(c[high] == x) {
		c.erase(c.begin() + high);
		return;
	}
	while(high > low + 1) {
		int mid = (high + low) / 2;
		if(c[mid] == x) {
			c.erase(c.begin() + mid);
			return;
		}
		if(c[mid] < x) low = mid;
		else high = mid;
	}
}

bool VertexFaceConnection::hasIntersection(int a, int b) const
{
	const std::deque<FaceIndex> &fa = m_connections[a];
	const std::deque<FaceIndex> &fb = m_connections[b];
	std::deque<FaceIndex>::const_iterator it = fa.begin();
	for(;it!=fa.end();++it) {
		if(hasIntersection(fb, *it)) return true;
	}
	return false;
}

bool VertexFaceConnection::hasIntersection(const std::deque<FaceIndex> &a, const FaceIndex &b) const
{
	std::deque<FaceIndex>::const_iterator it = a.begin();
	for(;it!=a.end();++it) {
		if(it->isConnectedTo(b)) return true;
		if(it->v0() > b.v2()) return false;
	}
	return false;
}

const std::deque<FaceIndex> &VertexFaceConnection::operator[](int v) const
{ return m_connections[v]; }

bool VertexFaceConnection::check(int vi) const
{
	const std::deque<FaceIndex> &faces = m_connections[vi];
	std::deque<FaceIndex>::const_iterator it = faces.begin();
	for(;it!=faces.end();++it) {
		const FaceIndex &fi = *it;
		if(!fi.hasV(vi) ) {
			std::cout<<"\n\n ERROR face"<<fi<< " not connected to vert "<<vi;
			return false;
		}
	}
	return true;
}

void VertexFaceConnection::getConnectedVertices(std::deque<int> &vs,
			int vi) const
{
	const std::deque<FaceIndex> &faces = m_connections[vi];
	std::deque<FaceIndex>::const_iterator it = faces.begin();
	for(;it!=faces.end();++it) {
		const FaceIndex &fi = *it;

		if(fi.v0() != vi) 
			addToVector(vs, fi.v0());

		if(fi.v1() != vi)
			addToVector(vs, fi.v1());
		
		if(fi.v2() != vi)
			addToVector(vs, fi.v2());
	}
}

void VertexFaceConnection::addToVector(std::deque<int> &vs, int x) const
{
	if(std::find(vs.begin(), vs.end(), x) == vs.end()) 
		vs.push_back(x);
}

bool VertexFaceConnection::getVertexOneRing(std::vector<int> &vertInds, int vi,
		const Vector3F *pos, const Vector3F &nv) const
{
	const std::deque<FaceIndex> &faces = m_connections[vi];
	const int nf = faces.size();
	if(nf < 3) return false;
	int va, vb, vc;
	if(!faces[0].getOppositeEdge(va, vb, vi) )
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

bool VertexFaceConnection::findNextVertex(int &c, const int &a, const int &b, const int &vi) const
{
	const std::deque<FaceIndex> &faces = m_connections[vi];
	std::deque<FaceIndex>::const_iterator it = faces.begin();
	for(;it!=faces.end();++it) {
		const FaceIndex &fi = *it;
		if(!fi.hasV(b)) continue;
		c = fi.getOpposite(b, vi);
		if(c > -1 && c != a)
			return true;
	}
	return false;
}

int VertexFaceConnection::totalSize() const
{
	int s = 0;
	std::vector<std::deque<FaceIndex> >::const_iterator it = m_connections.begin();
	for(;it!=m_connections.end();++it)
		s += it->size();
	return s;
}

void VertexFaceConnection::addVertexRingTo(std::vector<int> &vertInds, 
							const std::vector<int> &smallRing, int vi) const
{
	const std::deque<FaceIndex> &faces = m_connections[vi];
	std::deque<FaceIndex>::const_iterator it = faces.begin();
	for(;it!=faces.end();++it) {
		const FaceIndex &fi = *it;
		if(fi.v0() != vi) expandExclude(vertInds, smallRing, fi.v0());
		if(fi.v1() != vi) expandExclude(vertInds, smallRing, fi.v1());
		if(fi.v2() != vi) expandExclude(vertInds, smallRing, fi.v2());
	}
}

void VertexFaceConnection::expandExclude(std::vector<int> &a, const std::vector<int> &b, int vi) const
{
	if(std::find(std::begin(b), std::end(b), vi) != std::end(b))
		return;

	if(std::find(std::begin(a), std::end(a), vi) == std::end(a))
		a.push_back(vi);
}

}
