/*
 *  FrontDistanceMeasure.h
 *  aloe
 *
 *  Created by zhang on 18-2-24.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SDS_FRONT_DISTANCE_MEASURE_H
#define ALO_SDS_FRONT_DISTANCE_MEASURE_H

#include <boost/scoped_array.hpp>
#include <math/Vector3F.h>
#include <math/miscfuncs.h>

namespace alo {

namespace sds {

template<typename Ts>
class FrontDistanceMeasure {

/// front direction in 8 subcells
    Vector3F m_cellDirection[8];
/// transform into [-1,1]
    float m_offsetScale[4];
    boost::scoped_array<Ts> m_samples;
    int m_numSamples;
    
public:

    FrontDistanceMeasure();
    
    void attach(const float* coord,
                const SpaceFillingVector<Ts>& fronts,
                const int* frontRange,
                const SpaceFillingVector<Ts>& samples,
                const int* sampleRange);
    
    float measureAt(float x, float y, float z) const;
    
private:
/// p [-1,1]
/// return [0,7]
    int localCellInd(const float& px, const float& py, const float& pz) const;
    void toLocalSpace(float* p) const;
    
};

template<typename Ts>
FrontDistanceMeasure<Ts>::FrontDistanceMeasure(): m_numSamples(0)
{}

template<typename Ts>
void FrontDistanceMeasure<Ts>::attach(const float* coord,
                const SpaceFillingVector<Ts>& fronts,
                const int* frontRange,
                const SpaceFillingVector<Ts>& samples,
                const int* sampleRange)
{
    m_offsetScale[0] = -coord[0];
    m_offsetScale[1] = -coord[1];
    m_offsetScale[2] = -coord[2];
    m_offsetScale[3] = 1.f / coord[3];
    
    Vector3F meanNml(0.f,0.f,0.f);
    for(int i=frontRange[0];i<frontRange[1];++i) {
        meanNml += fronts[i]._nml;
    }
    meanNml.normalize();
    
    for(int i=0;i<8;++i) {
        m_cellDirection[i] = meanNml;
    }
    
    for(int i=frontRange[0];i<frontRange[1];++i) {
        Vector3F localP = fronts[i]._pos;
        toLocalSpace((float*)&localP);
        int j = localCellInd(localP.x, localP.y, localP.z);
        m_cellDirection[j] = fronts[i]._nml;
    }
    
    const int oldNs = m_numSamples;
    
    m_numSamples = sampleRange[1] - sampleRange[0] - 1;
    
    if(oldNs < m_numSamples) 
        m_samples.reset(new Ts[m_numSamples]);
    
/// first one is outside of box
    const int sampleBegin = sampleRange[0] + 1;
    for(int i=0;i<m_numSamples;++i) {
        m_samples[i] = samples[sampleBegin + i];
        toLocalSpace((float*)&m_samples[i]._pos);
    }
    
}

template<typename Ts>
int FrontDistanceMeasure<Ts>::localCellInd(const float& px, const float& py, const float& pz) const
{
    int x = px > 0.f ? 1 : 0;
    int y = py > 0.f ? 1 : 0;
    int z = pz > 0.f ? 1 : 0;
    return ((z<<2) | (y<<1) |  x);
    
}

template<typename Ts>
void FrontDistanceMeasure<Ts>::toLocalSpace(float* p) const
{
    p[0] += m_offsetScale[0]; p[0] *= m_offsetScale[3];
    p[1] += m_offsetScale[1]; p[1] *= m_offsetScale[3];
    p[2] += m_offsetScale[2]; p[2] *= m_offsetScale[3];
}

template<typename Ts>
float FrontDistanceMeasure<Ts>::measureAt(float x, float y, float z) const
{
    int j = localCellInd(x, y, z);
    const Vector3F& nml = m_cellDirection[j];
    
    float minD = 1e8f;
    float abminD = 1e8f;
    float d, abd;
    for(int i=0;i<m_numSamples;++i) {
    
        Vector3F vtomp(x, y, z);
        vtomp -= m_samples[i]._pos;
        
        d = vtomp.dot(nml);

/// closest to zero 
        abd = Absolute<float>(d);
        if(abminD > abd ) {
            minD = d;
            abminD = abd;
        }
    }
    
/// thickness
    return minD;
}

}

}
#endif
