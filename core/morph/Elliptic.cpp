/*
 *  Elliptic.cpp
 *  gorse
 *
 *  three border lines
 *  2019/8/13
 */
 
#include "Elliptic.h"
#include "LeafProfile.h"
#include <math/Hermite.h>

namespace alo {

namespace morph {
    
int Elliptic::sRegion[2][2] = {{0,1}, {1,2}};
bool Elliptic::sReversed[2][2] = {{false, true}, {false, true}};
    
Elliptic::Elliptic()
{}

Elliptic::~Elliptic()
{}

void Elliptic::create(LeafProfile &prof)
{
    const int &nu = prof._resolution.x;
    int petioleNu = nu * prof._petiolePortion.x;
    if(petioleNu < 1) petioleNu = 1;
    const float oneOverNu = 1.f / (float)nu;
    const float bladeDu = prof._size.x * (1.f - prof._petiolePortion.x) * oneOverNu;
    const float oneOverPetioleNu = 1.f / (float)petioleNu;
    const float petioleDu = prof._size.x * prof._petiolePortion.x * oneOverPetioleNu;
    const float &sy = prof._size.y;
    const float &sz = prof._size.z;
    const float upoblx = prof._oblique.x;
    const float upobly = 1.f + prof._oblique.y;
    const float downoblx = - prof._oblique.x;
    const float downobly = 1.f - prof._oblique.y;
    const float bladeBegin = prof._size.x * prof._petiolePortion.x;
    const float bladeWidth = prof._size.y * prof._petiolePortion.y * .5f;
    
    Hermite<Vector3F> herm;
    Vector3F hemp[2];
    hemp[0].set(0.f, 0.f, 0.f);
    hemp[1].set(1.f, 0.f, 0.f);
    Vector3F hemt[2];
    hemt[0].set(-.1f, 1.99f, -.3f);
    hemt[1].set(.3f, -1.33f, .3f);
    
    m_positions.clear();
    Vector3F q, vh;
    
    int nvert = 0;
    m_border[0].x = 0;
    for(int i=0;i<=petioleNu;++i) {
        q.x = petioleDu * i;
        q.y = -bladeWidth;
        q.z = 0.f;
        Matrix33F lr = getLocalRollRotation(oneOverPetioleNu * prof._petiolePortion.x * i, prof._twistBendRoll);
        lr.transformInPlace(q);
        m_positions.push_back(q);
        nvert++;
    }
    for(int i=1;i<nu;++i) {
        vh = herm.interpolate(oneOverNu * i, hemp, hemt);
        q.x = bladeBegin + bladeDu * i + bladeDu * downoblx;
        q.y = -sy * vh.y * downobly;
        q.z = sz * vh.z;
        Matrix33F lr = getLocalRotation(prof._petiolePortion.x + (1.f - prof._petiolePortion.x) * oneOverNu * i, prof._twistBendRoll);
        lr.transformInPlace(q);
        m_positions.push_back(q);
        nvert++;
    }
    m_border[0].y = nvert;
    
    m_border[1].x = nvert;
    for(int i=0;i<=petioleNu;++i) {
        q.x = petioleDu * i;
        q.y = 0.f;
        q.z = 0.f;
        Matrix33F lr = getLocalRollRotation(oneOverPetioleNu * prof._petiolePortion.x * i, prof._twistBendRoll);
        lr.transformInPlace(q);
        m_positions.push_back(q);
        nvert++;
    }
    for(int i=1;i<=nu;++i) {
        vh = herm.interpolate(oneOverNu * i, hemp, hemt);
        q.x = bladeBegin + bladeDu * i;
        q.y = 0.f;
        q.z = 0.f;
        Matrix33F lr = getLocalRotation(prof._petiolePortion.x + (1.f - prof._petiolePortion.x) * oneOverNu * i, prof._twistBendRoll);
        lr.transformInPlace(q);
        m_positions.push_back(q);
        nvert++;
    }
    m_border[1].y = nvert;
    
    m_border[2].x = nvert;
    for(int i=0;i<=petioleNu;++i) {
        q.x = petioleDu * i;
        q.y = bladeWidth;
        q.z = 0.f;
        Matrix33F lr = getLocalRollRotation(oneOverPetioleNu * prof._petiolePortion.x * i, prof._twistBendRoll);
        lr.transformInPlace(q);
        m_positions.push_back(q);
        nvert++;
    }
    for(int i=1;i<nu;++i) {
        vh = herm.interpolate(oneOverNu * i, hemp, hemt);
        q.x = bladeBegin + bladeDu * i + bladeDu * upoblx;
        q.y = sy * vh.y * upobly;
        q.z = sz * vh.z;
        Matrix33F lr = getLocalRotation(prof._petiolePortion.x + (1.f - prof._petiolePortion.x) * oneOverNu * i, 
            prof._twistBendRoll);
        lr.transformInPlace(q);
        m_positions.push_back(q);
        nvert++;
    }
    m_border[2].y = nvert;
    
}

int Elliptic::numBorders() const
{ return 3; }

const Int2 &Elliptic::border(const int &i) const
{ return m_border[i]; }

const Vector3F &Elliptic::position(const int &i) const
{ return m_positions[i]; }

int Elliptic::numRegions() const
{ return 2; }

void Elliptic::getRegion(int &b0, bool &r0, int &b1, bool &r1, const int &i) const
{
    b0 = sRegion[i][0]; b1 = sRegion[i][1];
    r0 = sReversed[i][0]; r1 = sReversed[i][1];
}

Matrix33F Elliptic::getLocalRotation(const float &alpha, const Float3 &twistBendRoll) const
{
    Quaternion t(twistBendRoll.x * alpha, Vector3F::XAxis);
    Matrix33F mat(t);
    Quaternion b(twistBendRoll.y * alpha, Vector3F::ZAxis);

    mat *= Matrix33F(b);

    Vector3F front = Vector3F::YAxis;
    mat.transformInPlace(front);
    Quaternion r(twistBendRoll.z * alpha, front);

    return mat * Matrix33F(r);
}

Matrix33F Elliptic::getLocalRollRotation(const float &alpha, const Float3 &twistBendRoll) const
{
    Quaternion r(twistBendRoll.z * alpha, Vector3F::YAxis);

    return Matrix33F(r);
}

}

}