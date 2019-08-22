/*
 *  SynthesisBundle.h
 *  gorse
 *
 *  instead of synthesizing dependent instances every time
 *  create a number of bundles and randomly pick one
 *  and modify the transforms accordingly
 *  2019/8/22
 */
 
#ifndef ALO_SCATTER_SYNTHESIS_BUNDLE_H
#define ALO_SCATTER_SYNTHESIS_BUNDLE_H

#include <math/SimpleBuffer.h>
#include <math/Matrix44F.h>
#include <math/RandomSelect.h>
#include <smp/SampleFilter.h>
#include <rng/Uniform.h>
#include <rng/Lehmer.h>

namespace alo {
    
namespace sca {
    
template<int Nbundle>
class SynthesisBundle {

    SimpleBuffer<int> m_objectIds[Nbundle];
    SimpleBuffer<Matrix44F> m_tms[Nbundle];
                    
public:

    SynthesisBundle();
    virtual ~SynthesisBundle();

/// draw Ts samples by Tr rule
/// get instance id and tm for all bundles
    template<typename Ts, typename Tr, typename Trng>
    void synthesize(const smp::SampleFilter<Ts> *filter,
                    Tr &rule,
                    const RandomSelect &selector,
                    Trng *rng);

    void transformABundle(SimpleBuffer<int> *objectIds,
                            SimpleBuffer<Matrix44F> *objectTms,
                            const Matrix44F *worldTm,
                            Uniform<Lehmer> *rng) const;
    
protected:

private:
    template<typename Ts, typename Tr, typename Trng>
    void synthesizeABudle(SimpleBuffer<int> &objectIds,
                    SimpleBuffer<Matrix44F> &tms,
                    const smp::SampleFilter<Ts> *filter,
                    Tr &rule,
                    const RandomSelect &selector,
                    Trng *rng);
                    
};

template< int Nbundle>
SynthesisBundle<Nbundle>::SynthesisBundle()
{}

template< int Nbundle>
SynthesisBundle<Nbundle>::~SynthesisBundle()
{}

template< int Nbundle>
template<typename Ts, typename Tr, typename Trng>
void SynthesisBundle<Nbundle>::synthesize(const smp::SampleFilter<Ts> *filter,
                                Tr &rule, const RandomSelect &selector,
                                Trng *rng)
{
    for(int i=0;i<Nbundle;++i) {
        synthesizeABudle(m_objectIds[i], m_tms[i],
                    filter, rule, selector, rng);
    }
}

template< int Nbundle>
template<typename Ts, typename Tr, typename Trng>
void SynthesisBundle<Nbundle>::synthesizeABudle(SimpleBuffer<int> &objectIds,
                    SimpleBuffer<Matrix44F> &tms,
                    const smp::SampleFilter<Ts> *filter,
                    Tr &rule,
                    const RandomSelect &selector,
                    Trng *rng)
{
    SimpleBuffer<Ts> subset;
    filter-> template drawSamples<Tr>(subset, rule);
    
    objectIds.purgeBuffer();
    tms.purgeBuffer();
    
    const int n = subset.count();
    
    Matrix44F tm;
    for(int i=0;i<n;++i) {
        int j = selector. template select<Trng>(rng);
        
        objectIds << j;

        const Ts &si = subset[i];
		
        tm.setTranslation(si._pos);
		
		Matrix33F r = rule.getRotation(si);
		tm.setRotation(r);
        
        tms << tm;
        
    }
}

template< int Nbundle>
void SynthesisBundle<Nbundle>::transformABundle(SimpleBuffer<int> *objectIds,
                            SimpleBuffer<Matrix44F> *objectTms,
                            const Matrix44F *worldTm,
                            Uniform<Lehmer> *rng) const
{
    
    const int sel = rng->rand(Nbundle);
    const SimpleBuffer<int> &selIds = m_objectIds[sel];
    const SimpleBuffer<Matrix44F> &selTms = m_tms[sel];
    
    const int &n = selIds.count();
    for(int i=0;i<n;++i) {
        if(rng->randf1() > .9f) continue;
        *objectIds << selIds[i];
        
        Matrix44F stm = selTms[i].transformBy(*worldTm);
        
        Matrix33F r = stm.rotation();
        Matrix33F::rotateUpToAlignLimited(r, Vector3F::YAxis, .7f);
		stm.setRotation(r);
        
        *objectTms << stm;
    }
}
    
}
    
}

#endif
