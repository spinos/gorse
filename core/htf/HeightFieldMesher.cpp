/*
 * HeightFieldMesher.cpp
 * aloe
 *
 *
 * 2019/8/8
 */
 
#include "HeightFieldMesher.h"

namespace alo {

HeightFieldMesher::HeightFieldMesher()
{}

HeightFieldMesher::~HeightFieldMesher()
{}

void HeightFieldMesher::attach(AdaptableMesh *msh)
{ m_mesh = msh; }

void HeightFieldMesher::triangulate(const HeightField &fld)
{
    const int offset = m_mesh->numVertices();
    int nhor, nver;
    fld.getResolution(nhor, nver);
    for(int j=0;j<nver;++j) {
        for(int i=0;i<nhor;++i) {
            Vector3F p = fld.vertex(i, j);
            m_mesh->addVertex(p);
        }
    }
    for(int j=0;j<nver-1;++j) {
        for(int i=0;i<nhor-1;++i) {
            int v0 = offset + j * nhor + i;
            int v1 = offset + j * nhor + i + 1;
            int v2 = offset + (j + 1) * nhor + i + 1;
            int v3 = offset + (j + 1) * nhor + i;
            m_mesh->addQuad(v0, v3, v2, v1);
        }
    }
}

void HeightFieldMesher::detach()
{
    m_mesh->calculateVertexNormals();
}

}
