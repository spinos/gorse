/*
 *  GeodesicDistance.cpp
 *  alo
 *
 */
 
#include "GeodesicDistance.h"
#include "VertexValue.h"
#include <geom/V1ATriangleMesh.h>
#include <algorithm>

namespace alo {
    
GeodesicDistance::GeodesicDistance()
{}

GeodesicDistance::~GeodesicDistance()
{}

void GeodesicDistance::buildTopology(const ver1::ATriangleMesh *mesh)
{
    MeshTopology::buildTopology(mesh);
    const int &n = mesh->numVertices();
    m_vertexEdgeConnection.resize(n);
    EdgeDataType *block = firstEdge();
    while(block) {
        
        for(int i=0;i<block->count();++i) {
            const EdgeIndex &k = block->key(i);
            m_vertexEdgeConnection.connectEdge(k);
        }
        
        block = nextEdge(block);
    }
	
    calcEdgeLength(mesh);
    
    m_gradient.resetBuffer(numFaces());
    indexFaces();
}

void GeodesicDistance::calcEdgeLength(const ver1::ATriangleMesh *mesh)
{
    const Vector3F *pos = mesh->c_positions();
    EdgeDataType *block = firstEdge();
    while(block) {
        
        for(int i=0;i<block->count();++i) {
            const EdgeIndex &k = block->key(i);
            EdgeValue &e = block->value(i);
            e.setLength(pos[k.v0()].distanceTo(pos[k.v1()]));
        }
        
        block = nextEdge(block);
    }
}

int GeodesicDistance::closestVertexTo(const Vector3F& ori, const ver1::ATriangleMesh *mesh) const
{
    const Vector3F *pos = mesh->c_positions();
    const int &n = mesh->numVertices();
    int r = 0;
    float minD = pos[0].distanceTo(ori);
    for(int i=1;i<n;++i) {
        float d = pos[i].distanceTo(ori);
        if(minD > d) {
            minD = d;
            r = i;
        }
    }
    return r;
}

void GeodesicDistance::calcDistanceFromVertex(int ori, const ver1::ATriangleMesh *mesh)
{
    initField();
    VertexValue &v = vertex(ori);
    v.lock();
    v.setCost(0.f);
    
    std::vector<int> trials;
    trials.push_back(ori);
    
    while (trials.size() > 0) {
        
        int i = trials.front();
        propagate(trials);
        
        trials.erase(trials.begin() ); 
    }
}
    
void GeodesicDistance::normalizeDistance(const ver1::ATriangleMesh *mesh)
{  
    const int &n = mesh->numVertices();
    int r = 0;
    float maxD = vertex(0).cost();
    for(int i=1;i<n;++i) {
        float d = vertex(i).cost();
        if(maxD < d) {
            maxD = d;
            r = i;
        }
    }
    
    const float s = 1.f / maxD;
    
    for(int i=0;i<n;++i) {
        VertexValue &v = vertex(i);
        float d = v.cost();
        v.setCost(d * s);
    }
}

void GeodesicDistance::initField()
{
    const int &n = numVertices();
    for(int i=0;i<n;++i) {
        VertexValue &v = vertex(i);
        v.unlock();
        v.setCost(1e28f);
    }
}

void GeodesicDistance::propagate(std::vector<int> &trials)
{
    const int i = trials.front();
    VertexValue &vi = vertex(i);
    vi.lock();
    
    const std::deque<EdgeIndex> &neis = m_vertexEdgeConnection[i];
    std::deque<EdgeIndex>::const_iterator it = neis.begin();
    for(;it!=neis.end();++it) {
        const EdgeIndex &ei = *it;
        
        int j = ei.v0();
        if(j == i) j = ei.v1();
        
        VertexValue &vj = vertex(j);
        
        EdgeValue *e = edge(ei);
        const float l = vi.cost() + e->length();
        if(vj.cost() > l) vj.setCost(l);
        
        if(vj.isLocked()) continue;
        
        if(std::find(trials.begin(), trials.end(), j) == trials.end()) {
            trials.push_back(j);
        }
    }
}

void GeodesicDistance::deform(ver1::ATriangleMesh *mesh, const Vector3F &displacement) const
{
    Vector3F *pos = mesh->positions();
    
    const int &n = numVertices();
    for(int i=0;i<n;++i) {
        const VertexValue &v = vertex(i);
        const float &w = v.cost();
        pos[i] += displacement * (w * w);
    }
}

void GeodesicDistance::calcGradient(const ver1::ATriangleMesh *mesh)
{
    const Vector3F *pos = mesh->c_positions();
    FaceDataType *block = firstFace();
	while(block) {
		for(int i=0;i<block->count();++i) {
            const FaceIndex &fi = block->key(i);
			const float &d0 = vertex(fi.v0()).cost();
            const float &d1 = vertex(fi.v1()).cost();
            const float &d2 = vertex(fi.v2()).cost();
            const Vector3F &p0 = pos[fi.v0()];
            const Vector3F &p1 = pos[fi.v1()];
            const Vector3F &p2 = pos[fi.v2()];
            
            const FaceValue &f = block->value(i);
			Vector3F &grad = m_gradient[f.ind()];
			
            CalcFaceGradient(grad, d0, d1, d2, p0, p1, p2);
		}
		block = nextFace(block);
	}
}

void GeodesicDistance::CalcFaceGradient(Vector3F &grad, const float &d0, const float &d1, const float &d2,
                const Vector3F &p0, const Vector3F &p1, const Vector3F &p2)
{
    const float oneOver2A = .5f / ver1::ATriangleMesh::CalcArea(p0, p1, p2);
    const Vector3F ascend20 = ver1::ATriangleMesh::PerpendicularToOppositeEdge(p1, p2, p0);
    const Vector3F ascend01 = ver1::ATriangleMesh::PerpendicularToOppositeEdge(p2, p0, p1);
    grad = ascend20 * (oneOver2A * (d1 - d0)) + ascend01 * (oneOver2A * (d2 - d0));
}

}
