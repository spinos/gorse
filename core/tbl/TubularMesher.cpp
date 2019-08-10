/*
 *  TubularMesher.cpp
 *  gorse
 *
 *  2019/8/10
 */

#include "TubularMesher.h"
#include <geom/AdaptableMesh.h>

namespace alo {

TubularMesher::TubularMesher()
{}

TubularMesher::~TubularMesher()
{}

void TubularMesher::attach(AdaptableMesh *mesh)
{ m_mesh = mesh; }

void TubularMesher::detach()
{ m_mesh->calculateVertexNormals(); }

void TubularMesher::triangulate(const TubularCrossSection &cs)
{
	
}

}
