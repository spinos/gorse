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

void TubularProfile::begin(const Vector3F &p)
{
    m_p0 = p;
    m_disp.clear();
}

void TubularProfile::operator<<(const Vector3F &p)
{ m_disp.push_back(p); }

void TubularProfile::end()
{
    const int n = numSegments();
    for(int i=0;i<n;++i) {
        std::cout << "\n dv" << i << m_disp[i];
    }

    Vector3F side = m_disp[0]; side.normalize();
    Vector3F up = m_disp[0].cross(m_disp[1]); up.normalize();
    Vector3F front = side.cross(up);
    m_frm0.fill(side, up, front);
    std::cout << "\n init frame " <<m_frm0;

    m_rot.resize(n);

    Matrix33F frm = m_frm0;
    Matrix33F invfrm;
    
    for(int i=0;i<n-1;++i) {
        std::cout << "\n\n segment " << i;

        Vector3F lside = m_disp[i].normal();
        invfrm = frm; invfrm.inverse();

        invfrm.transformInPlace(lside);
        std::cout << "\n local_side0 " << lside;

        lside = m_disp[i+1].normal();
        invfrm.transformInPlace(lside);
        std::cout << "\n local_side1 " << lside;
        lside.y = 0.f; lside.normalize();

        float pitch = acos(lside.x);
        if(lside.z > 0.f) pitch = -pitch;
        std::cout << "\n pitch " << pitch;
        m_rot[i].x = pitch;

        up.set(0.f, 1.f, 0.f);
        frm.transformInPlace(up);

        Quaternion pitchi(pitch, up);
        frm *= Matrix33F(pitchi);
        
        invfrm = frm; invfrm.inverse();

        lside = m_disp[i+1].normal();
        invfrm.transformInPlace(lside);

        std::cout << "\n local_side1 after pitch " << lside;

        lside.z = 0.f; lside.normalize();

        float yaw = acos(lside.x);
        if(lside.y < 0.f) yaw = -yaw;
        std::cout << "\n yaw " << yaw;
        m_rot[i].y = yaw;

        front.set(0.f, 0.f, 1.f);
        frm.transformInPlace(front);

        Quaternion yawi(yaw, front);
        frm *= Matrix33F(yawi);

    }
    m_rot[n-1] = Float2(0.f, 0.f);

}

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

}
