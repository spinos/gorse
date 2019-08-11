/*
 *  TubularCrossSection.cpp
 *  gorse
 *
 *  2019/8/10
 */

#include "TubularCrossSection.h"
#include <math/Polygon2D.h>
#include <math/EllipseFunc.h>
#include <math/Vector3F.h>

namespace alo {

TubularCrossSection::TubularCrossSection()
{ m_lass = new Polygon2D; }

TubularCrossSection::~TubularCrossSection()
{ delete m_lass; }

void TubularCrossSection::create(int nu, float a, float b)
{
	EllipseFunc ellip;
    ellip.setA(a);
    ellip.setB(b);

    const float da = 6.28318531f / (float)nu;

    m_lass->reset();

    for(int i=0;i<nu;++i) {
        float phi = da * i;
        float radius = ellip.polarForm(phi);
        (*m_lass) << Float2(radius * cos(phi), radius * sin(phi) );
    }

    m_lass->finish();
}

int TubularCrossSection::numVertices() const
{ return m_lass->numSegments(); }

void TubularCrossSection::getVertex(float *q, int i) const
{
    const Float2 &vi = m_lass->vertex(i);
    q[0] = 0.f;
    q[1] = vi.x;
    q[2] = vi.y;
}

}
