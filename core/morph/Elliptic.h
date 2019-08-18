/*
 *  Elliptic.h
 *  gorse
 *
 *  three border lines
 *  2019/8/13
 */
 
#ifndef ALO_MORPH_ELLIPTIC_H
#define ALO_MORPH_ELLIPTIC_H

#include <math/Int2.h>
#include <math/Vector3F.h>
#include <vector>

namespace alo {

struct Float3;
class Matrix33F;

namespace morph {
    
struct LeafProfile;
    
class Elliptic {
    
    Int2 m_border[3];
    std::vector<Vector3F> m_positions;
    
public:

    Elliptic();
    virtual ~Elliptic();
    
    void create(LeafProfile &prof);
    
    int numBorders() const;
/// (begin, end) of i-th border
    const Int2 &border(const int &i) const;
    const Vector3F &position(const int &i) const;
    
    int numRegions() const;
    void getRegion(int &b0, bool &r0, int &b1, bool &r1, const int &i) const;
    
protected:

    Matrix33F getLocalRotation(const float &alpha, const Float3 &twistBendRoll) const;
    Matrix33F getLocalRollRotation(const float &alpha, const Float3 &twistBendRoll) const;

private:

    static int sRegion[2][2];
    static bool sReversed[2][2];

};

}

}

#endif
