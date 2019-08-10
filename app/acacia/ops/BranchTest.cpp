/*
 *  BranchTest.cpp
 *  acacia
 *
 */

#include "BranchTest.h"
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>
#include <qt_ogl/DrawableResource.h>
#include <sds/SpaceFillingVector.h>
#include <sds/FZOrder.h>
#include <sds/SparseFieldBuilder.h>
#include <sds/SparseFieldBuildRule.h>
#include <geom/AdaptableMesh.h>
#include <mesh/randomRibbon.h>
#include <rng/Lehmer.h>
#include <rng/Uniform.h>
#include <smp/Triangle.h>
#include <smp/SurfaceGeodesicSample.h>
#include <sds/SparseCubicField.h>
#include <gdf/HexahedronDistanceBuilder.h>
#include <gdf/HexahedronDistanceBuildRule.h>
#include <sds/SparseFieldGradientLookup.h>
#include <sds/SparseFieldGradientLookupRule.h>
#include <tbl/TubularProfile.h>
#include <ctime>

namespace alo {
   
BranchTest::BranchTest() 
{
    std::time_t secs = std::time(0);
	Uniform<Lehmer> lmlcg(secs);
    
    m_mesh = new AdaptableMesh;
    createRandomRibbon<Uniform<Lehmer> >(*m_mesh, 7, 101, &lmlcg);
    
    TubularProfile tbprof;
    tbprof.begin(Vector3F::Zero);
    tbprof<<Vector3F(0.01f, 1.99f, 0.01f);
    tbprof<<Vector3F(.76f, 1.19f, 0.12f);
    tbprof<<Vector3F(.69f, .3f, 0.5f);
    tbprof<<Vector3F(.3f, .15f, .77f);
    tbprof<<Vector3F(.2f, .5f, .57f);
    tbprof<<Vector3F(.1f, .01f, .67f);
    tbprof<<Vector3F(.05f, -.1f, .47f);
    
    tbprof.end();
#if 0
    BoundingBox shapeBox;
    m_mesh->getAabb(shapeBox);
    const float span = shapeBox.getLongestDistance();
    shapeBox.expand(span * .001f);
    shapeBox.round();
    std::cout<<"\n shape box"<<shapeBox;
    
    const float ssz = span * .001f;
	std::cout << "\n sample size " << ssz;
    
    typedef SurfaceGeodesicSample SampleTyp;
    typedef SampleInterp<SurfaceGeodesicSample> SampleInterpTyp;
    typedef smp::Triangle<SampleTyp > SamplerTyp;
    SamplerTyp sampler;
	sampler.setSampleSize(ssz);
    
    typedef sds::SpaceFillingVector<SampleTyp> PntArrTyp;
	PntArrTyp pnts;
    
    SampleTyp asmp;
    SampleInterpTyp interp;
    const int nt = m_mesh->numTriangles();
    for(int i=0;i<nt;++i) {
        m_mesh->getTriangle<SampleTyp, SamplerTyp >(sampler, i);
        sampler.addSamples <PntArrTyp, SampleInterpTyp, Uniform<Lehmer> >(pnts, asmp, interp, &lmlcg);
    }
    const int ns = pnts.size();
    std::cout<<"\n n samples "<<ns;
    
    sds::FZOrderCurve sfc;
    typedef gdf::HexahedronDistanceBuildRule<sds::FZOrderCurve> FieldRuleTyp;
    FieldRuleTyp rule(&sfc);
    rule.setP(6);
    rule.setBBox(shapeBox);
    
    typedef sds::SparseCubicField<float> FieldTyp;
    FieldTyp field;
    gdf::HexahedronDistanceBuilder<FieldTyp> builder;

    builder.attach<FieldRuleTyp>(&field, rule);
    builder.measure<PntArrTyp, FieldRuleTyp>(pnts, rule);    
    builder.detach<FieldRuleTyp>(rule);

    typedef sds::SparseFieldGradientLookupRule<sds::FZOrderCurve> LookupRuleTyp;
    LookupRuleTyp lookupRule(&sfc);
    lookupRule.setP(6);
    lookupRule.setBBox(shapeBox);

    sds::SparseFieldGradientLookup<FieldTyp> reader;
    reader.attach(&field);

    Vector3F sum(0,0,0);
    int nupd = 0;

    for(int i=0;i<ns;++i) {
        const float *p = (const float *)&pnts[i]._pos;
        reader.lookupGradient<LookupRuleTyp>(p, lookupRule);
        if(reader.isStatusUnknown()) continue;

        if(reader.isStatusUpdate()) nupd++;

        pnts[i]._geod = reader.interpolate(p);
        reader.interpolateGradient((float *)&pnts[i]._grad, p);

        sum += pnts[i]._grad;
    }

    sum *= 1.f / ns;

    std::cout << "\n avg grad " << sum
                << "\n n update " << nupd;

    reader.detach();
#endif
    DrawableResource *rec = createResource();
    setResource(rec);
}

BranchTest::~BranchTest()
{}
    
void BranchTest::update()
{
    computeMesh();
}

void BranchTest::addDrawableTo(DrawableScene *scene)
{
    setDrawableScene(scene);
    computeMesh();  
}

void BranchTest::computeMesh()
{
    DrawableResource *rec = resource();

    lockScene();
    const int beforeFrame = frameNumber() - 2;
    if(rec->changedOnFrame() > beforeFrame) {
/// prevent editing unsynchronized resource
        unlockScene();
        return;
    }

    UpdateMeshResouce(rec, m_mesh);

    processResourceNoLock(rec);
    unlockScene();

    setBound(m_mesh);
}

}
