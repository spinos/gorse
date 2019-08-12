/*
 *  LeafProfile.h
 *  gorse
 *
 *  to create a leaf
 *  2019/8/13
 */
 
#ifndef ALO_MORPH_LEAF_PROFILE_H
#define ALO_MORPH_LEAF_PROFILE_H

#include <math/Int2.h>
#include <math/Float2.h>
#include <math/Float3.h>
#include <math/Matrix33F.h>

namespace alo {

namespace morph {

struct LeafProfile {

    Int2 _resolution;
    Float2 _oblique;
    Float3 _twistBendRoll;
    Float3 _size;
    Matrix33F _rotation;
    Vector3F _position;

};

}

}

#endif
