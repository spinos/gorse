/*
 * HeightField.cpp
 * aloe
 *
 *
 * 2019/8/8
 */
 
#include "HeightField.h"
#include <math/Vector3F.h>
#include <math/Bilinear.h>
#include <math/Polygon2D.h>

namespace alo {

HeightField::HeightField() : m_gridSize(1.f),
m_heightScale(1.f)
{
    m_origin[0] = m_origin[1] = m_origin[2] = 0.f;
}

HeightField::~HeightField()
{}

void HeightField::setGridSize(float x)
{ m_gridSize = x; }

void HeightField::setHeightScale(float x)
{ m_heightScale = x; }

void HeightField::setOrigin(const float *x)
{
    m_origin[0] = x[0];
    m_origin[1] = x[1];
    m_origin[2] = x[2];
}

void HeightField::setValue(const Bilinear<float> &fval, const Polygon2D &region)
{
    const int &nr = numRows();
    const int &nc = numCols();
    for(int i=0;i<nc;++i) {
        float *c = column(i);
        const float x = (float)i / (float)nc;
        for(int j=0;j<nr;++j) {
            const Float2 q(m_origin[0] + m_gridSize * i, m_origin[2] + m_gridSize * j);
            if(region.isPointInside(q)) {
                const float y = (float)j / (float)nr;
                c[j] = fval.interpolate(x, y);
            }
        }
    }
}

void HeightField::smooth()
{
    Array2<float> tmp;
    const float k3[3] = {.3f, .4f, .3f};
    tmp.copy(*this);
    convoluteVertical(tmp, k3);
    tmp.copy(*this);
    convoluteHorizontal(tmp, k3);
}

void HeightField::getResolution(int &h, int &v) const
{
    h = numCols();
    v = numRows();
}

Vector3F HeightField::vertex(const int &i, const int &j) const
{
    return Vector3F(m_origin[0] + m_gridSize * i, 
                    m_origin[1] + m_heightScale * column(i)[j],
                    m_origin[2] + m_gridSize * j);
}

}
