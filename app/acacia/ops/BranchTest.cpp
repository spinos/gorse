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
#include <smp/SurfaceSample.h>
#include <sds/SparseCubicField.h>
#include <gdf/HexahedronDistanceBuilder.h>
#include <gdf/HexahedronDistanceBuildRule.h>
#include <sds/SparseFieldGradientLookup.h>
#include <sds/SparseFieldGradientLookupRule.h>
#include <ctime>

namespace alo {
   
BranchTest::BranchTest() 
{
    std::time_t secs = std::time(0);
	Uniform<Lehmer> lmlcg(secs);
    
    m_mesh = new AdaptableMesh;
    createRandomRibbon<Uniform<Lehmer> >(*m_mesh, 7, 101, &lmlcg);
    
    BoundingBox shapeBox;
    m_mesh->getAabb(shapeBox);
    const float span = shapeBox.getLongestDistance();
    shapeBox.expand(span * .0019f);
    shapeBox.round();
    std::cout<<"\n shape box"<<shapeBox;
    
    const float ssz = span * .001f;
	std::cout << "\n sample size " << ssz;
    
    typedef smp::Triangle<SurfaceSample > SamplerTyp;
    SamplerTyp sampler;
	sampler.setSampleSize(ssz);
    
    typedef sds::SpaceFillingVector<SurfaceSample> PntArrTyp;
	PntArrTyp pnts;
    
    SurfaceSample asmp;
    SampleInterp interp;
    const int nt = m_mesh->numTriangles();
    for(int i=0;i<nt;++i) {
        m_mesh->getTriangle<SurfaceSample, SamplerTyp >(sampler, i);
        sampler.addSamples <PntArrTyp, SampleInterp, Uniform<Lehmer> >(pnts, asmp, interp, &lmlcg);
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

    for(int i=0;i<ns;++i) {
        const float *p = (const float *)&pnts[i]._pos;
        reader.lookupGradient<LookupRuleTyp>(p, lookupRule);
        if(reader.isStatusUnknown()) continue;

        float d = reader.interpolate(p);
        float g[3];
        reader.interpolateGradient(g, p);
        
    }

    reader.detach();

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
