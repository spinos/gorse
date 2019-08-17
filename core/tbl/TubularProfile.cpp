/*
 *  TubularCrossSection.cpp
 *  gorse
 *
 *  2019/8/10
 */

#include "TubularProfile.h"

namespace alo {

TubularProfile::TubularProfile()
{}

TubularProfile::~TubularProfile()
{}

void TubularProfile::begin(const Vector3F &p, const Matrix33F &frm)
{
    m_p0 = p;
    m_frm0 = frm;
    m_disp.clear();
}

void TubularProfile::operator<<(const Vector3F &p)
{ m_disp.push_back(p); }

void TubularProfile::end()
{
    const int n = numSegments();

    m_rot.resize(n);

    Matrix33F frm = m_frm0;
    Matrix33F invfrm;
    
    for(int i=0;i<n-1;++i) {
        //std::cout << "\n\n segment " << i;

        Vector3F lside = m_disp[i].normal();
        invfrm = frm; invfrm.inverse();

        invfrm.transformInPlace(lside);
        //std::cout << "\n local_side0 " << lside;

        lside = m_disp[i+1].normal();
        invfrm.transformInPlace(lside);
        //std::cout << "\n local_side1 " << lside;
        lside.y = 0.f; lside.normalize();

        float pitch = acos(lside.x);
        if(lside.z > 0.f) pitch = -pitch;
        //std::cout << "\n pitch " << pitch;
        m_rot[i].x = pitch;

        Vector3F up(0.f, 1.f, 0.f);
        frm.transformInPlace(up);

        Quaternion pitchi(pitch, up);
        frm *= Matrix33F(pitchi);
        
        invfrm = frm; invfrm.inverse();

        lside = m_disp[i+1].normal();
        invfrm.transformInPlace(lside);

        //std::cout << "\n local_side1 after pitch " << lside;

        lside.z = 0.f; lside.normalize();

        float yaw = acos(lside.x);
        if(lside.y < 0.f) yaw = -yaw;
        //std::cout << "\n yaw " << yaw;
        m_rot[i].y = yaw;

        Vector3F front(0.f, 0.f, 1.f);
        frm.transformInPlace(front);

        Quaternion yawi(yaw, front);
        frm *= Matrix33F(yawi);

    }
    m_rot[n-1] = Float2(0.f, 0.f);

}

void TubularProfile::smooth()
{
    int n = numSegments();
    for(int i=0;i<n;++i) {
        int j = i*2 + 1;
        m_disp.insert(m_disp.begin() + j, m_disp[j-1]);
    }

    n = numSegments();
    for(int i=0;i<n;++i) {
        m_disp[i] *= .5f;
    }

    for(int i=0;i<n;++i) {
        int i0 = i - 1;
        if(i0 < 0) i0 = 0;
        int i1 = i + 1;
        if(i1 > n - 1) i1 = n - 1;

        Vector3F p = m_disp[i0];
        Vector3F q = m_disp[i1];
        m_disp[i] = m_disp[i] * .5f + p * .25f + q * .25f;
    }
}

const Vector3F &TubularProfile::pos0() const
{ return m_p0; }

const Matrix33F &TubularProfile::frm0() const
{ return m_frm0; }

const Vector3F &TubularProfile::displacement(const int &i) const
{ return m_disp[i]; }

int TubularProfile::numSegments() const
{ return m_disp.size(); }

void TubularProfile::getSegment(Vector3F &pos, Matrix33F &frm, 
            Vector3F &disp, Float2 &pitchYaw,
            const int iseg) const
{
    pos = m_p0;
    frm = m_frm0;
    disp = m_disp[iseg];
    pitchYaw = m_rot[iseg];

    if(iseg < 1) return;

    for(int i=1;i<=iseg;++i) {
        pos += m_disp[i-1];
        
        Vector3F up(0.f, 1.f, 0.f);
        frm.transformInPlace(up);

        Quaternion pitchi(m_rot[i-1].x, up);
        frm *= Matrix33F(pitchi);

        Vector3F front(0.f, 0.f, 1.f);
        frm.transformInPlace(front);

        Quaternion yawi(m_rot[i-1].y, front);
        frm *= Matrix33F(yawi);

    }
}

Vector3F TubularProfile::interpolatePosition(const float &alpha) const
{
    const float por = numSegments() * alpha;
    Vector3F pos = m_p0;

    if(por < 1.f) {
        pos += m_disp[0] * por;
        return pos;
    }

    for(int i=1;i<numSegments();++i) {
        pos += m_disp[i-1];

        if((por - i) < 1.f) {
            pos += m_disp[i] * (por - i);
            break;
        }
    }

    return pos;
    
}

Matrix33F TubularProfile::interpolateRotation(const float &alpha) const
{
    const float por = numSegments() * alpha;
    Matrix33F frm = m_frm0;

    for(int i=0;i<numSegments();++i) {

        float pitchAng = m_rot[i].x;
        float yawAng = m_rot[i].y;
        if(por < 1.f) {
            pitchAng *= por;
            yawAng *= por;
        }

        Vector3F up(0.f, 1.f, 0.f);
        frm.transformInPlace(up);

        Quaternion pitchi(pitchAng, up);
        frm *= Matrix33F(pitchi);

        Vector3F front(0.f, 0.f, 1.f);
        frm.transformInPlace(front);

        Quaternion yawi(yawAng, front);
        frm *= Matrix33F(yawi);

        if((por - i) < 1.f) break;
    }

    return frm;
}

Matrix33F TubularProfile::calculateFrame0(const Vector3F &v0, const Vector3F &v1)
{
    Matrix33F mat;
    Vector3F side = v0; side.normalize();
    Vector3F up = v0.cross(v1); up.normalize();
    Vector3F front = side.cross(up);
    float fl = front.length();
    if(fl > 1e-6f) {
        front /= fl;
        up = front.cross(side);
        mat.fill(side, up, front);
    }
    return mat;
}

void TubularProfile::expandRadius(const float &x)
{
    
}

}
