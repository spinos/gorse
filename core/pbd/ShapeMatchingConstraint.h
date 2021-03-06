/*
 *  ShapeMatchingConstraint.h
 *  aloe
 *  
 *  2019/10/6
 */
 
#ifndef ALO_PBD_SHAPE_MATCHING_CONSTRAINT_H
#define ALO_PBD_SHAPE_MATCHING_CONSTRAINT_H

#include <math/Quaternion.h>

namespace alo {

class ShapeMatchingConstraint {
    
public:

    ShapeMatchingConstraint();
    
    void setOffset(const int x);
    void setNumVertices(const int x);
    void calculateMass(float *mass, const float *invMass, 
                    const int *ind);
    void calculateP(Vector3F *p,
                        const float *mass,
                        const Vector3F *positions,
                        const int *ind);
    void setQ(Vector3F *q, const Vector3F *p,
                        const int *ind);
    void calculateG(Vector3F *g,
                        const Vector3F *p, 
                        const Vector3F *q);
                        
    void solvePosition(Vector3F &pos, 
                    const float* q_n1, const int &vi,
                    const int& i, const Vector3F *g) const;
                      
    const float &stiffness() const;
    
    static float getMass(const float &invMass);
    
private:

    Quaternion _rotq;
    Vector3F _centerOfMass;
    float _totalMass;
    int _offset;
    int _numVertices;
    float _stiffness;
    int _padding;
    
};

}

#endif
