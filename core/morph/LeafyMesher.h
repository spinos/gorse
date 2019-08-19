/* 
 *  LeafyMesher.h
 *  gorse
 *
 *  2019/8/19
 */
 
#ifndef ALO_MORPH_LEAFY_MESHER_H
#define ALO_MORPH_LEAFY_MESHER_H

#include "PlantMesher.h"
#include "LeafProfile.h"
#include <mesh/RegionMesher.h>

namespace alo {

namespace morph {

template<typename T>
class LeafyMesher : public PlantMesher {

    T *m_rng;
    
public:

	LeafyMesher(T *rng);
	virtual ~LeafyMesher();

    template<typename Tleaf>
	void triangulateLeaf(const Plant &pl, LeafProfile &prof);
    
protected:

private:

    template<typename Tleaf>
    void triangulateLeaves(std::vector<Vector3F> positions,
                        std::vector<Matrix33F> rotations,
                        LeafProfile &prof);
                        
    Matrix33F calculateLeafRotation(const Matrix33F &budRotation, const bool isLateral);
    
};

template<typename T>
LeafyMesher<T>::LeafyMesher(T *rng)
{ m_rng = rng; }

template<typename T>
LeafyMesher<T>::~LeafyMesher()
{}

template<typename T>
template<typename Tleaf>
void LeafyMesher<T>::triangulateLeaf(const Plant &pl, LeafProfile &prof)
{
    const int n = pl.numStems();
    for(int i=0;i<n;++i) {
        const Stem *si = pl.stem(i);
        
        std::vector<Vector3F> positions;
        std::vector<Matrix33F> rotations;
        si->getAllBuds(positions, rotations);
        
        triangulateLeaves<Tleaf>(positions, rotations, prof);
    }

}

template<typename T>
template<typename Tleaf>
void LeafyMesher<T>::triangulateLeaves(std::vector<Vector3F> positions,
                        std::vector<Matrix33F> rotations,
                        LeafProfile &prof)
{
    const int n = positions.size();
    const int n1 = rotations.size();
    if(n != n1) return;
    
    RegionMesher mshr;
    mshr.attach(mesh());

    Tleaf elp;
    elp.create(prof);
    
    for(int i=0;i<n;++i) {
        bool isLateral = (i < (n-1));
        Matrix33F mat = calculateLeafRotation(rotations[i], isLateral);
        mshr. template triangulate<Tleaf>(elp, positions[i], mat);
    }
}

template<typename T>
Matrix33F LeafyMesher<T>::calculateLeafRotation(const Matrix33F &budRotation, const bool isLateral)
{
    Matrix33F mat = budRotation;
    if(isLateral) Matrix33F::rotateAroundLocalY(mat, 1.5f);
    else Matrix33F::rotateAroundLocalY(mat, .1f);
    
    Matrix33F invmat = mat; invmat.inverse();
    Vector3F localUp(0.f, 1.f, 0.f);
    invmat.transformInPlace(localUp);
    
    localUp.x = 0.f;
    if(localUp.length2() < 1e-8f) return mat;
    
    localUp.normalize();
    
    float ang = acos(localUp.z);
    if(localUp.y > 0.f) ang = -ang;

    Matrix33F::rotateAroundLocalX(mat, ang);
    
    return mat;
}

}

}

#endif
