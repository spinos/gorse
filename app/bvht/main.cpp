/*
 *  World/Local grid with bvh test
 *  2019/5/8
 */

#include <cstring>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <grd/LocalGrid.h>
#include <grd/LocalGridBuilder.h>
#include <grd/LocalGridBuildRule.h>
#include <grd/LocalGridLookupRule.h>
#include <grd/WorldGridBuilder.h>
#include <grd/WorldGridBuildRule.h>
#include <grd/WorldGridLookupRule.h>
#include <grd/TestCell.h>
#include <grd/GridInCell.h>
#include <sds/FZOrder.h>
#include <ctime>

using namespace alo;

void testBvh()
{
    grd::BoxObject *bo = new grd::BoxObject;
    bo->_bbox.setMin(-3.f, 0.f, -4.f);
    bo->_bbox.setMax( 3.f, 25.f,  4.f);
	
	typedef grd::ObjectInstancer<grd::TestInstance, grd::BoxObject> InstancerTyp;
    InstancerTyp *m_instancer = new InstancerTyp;
	
    m_instancer->addObject(bo);
	
	static const int udim = 100;
    static const int vdim = 100;
    static const float spacing = 23.9;
    static const float xzSpan = 2.5f;
    static const float ySpan = 0.1f;
    static const float coverOrigin = 0;
    static const float scaleSpan = .5f;
    
    std::time_t secs = std::time(0);
    m_instancer->createPhalanx(udim, vdim, spacing, xzSpan, ySpan, coverOrigin, scaleSpan, secs);
	
	sds::FZOrderCurve sfc;
    sfc.setCoord(32.f, 32.f, 32.f, 16.f);
    
    typedef grd::LocalGridBuildRule<sds::FZOrderCurve> CellBuildRuleTyp;
    CellBuildRuleTyp cellRule(&sfc);
    cellRule.setP(5);
	
	typedef grd::LocalGridBuilder<grd::LocalGrid<float> > CellBuilderTyp;
    CellBuilderTyp cellBuilder;
	
	typedef grd::GridInCell<grd::LocalGrid<float> > WorldCellTyp;

    typedef grd::WorldGrid<int, WorldCellTyp > WorldTyp;
    WorldTyp *m_worldGrid = new WorldTyp;
    
    typedef grd::WorldGridBuildRule<int, sds::FZOrderCurve > WorldRuleTyp;
    WorldRuleTyp *m_worldRule = new WorldRuleTyp;
    const int cencz[4] = {0,0,0,256};
    m_worldRule->setCenterCellSize(cencz);

    typedef grd::WorldGridBuilder<int, WorldCellTyp > WorldBuilderTyp;
    WorldBuilderTyp *m_worldBuilder = new WorldBuilderTyp;

    m_worldBuilder->attach(m_worldGrid);

    m_worldBuilder->addInstances<InstancerTyp, WorldRuleTyp, CellBuilderTyp, CellBuildRuleTyp >(*m_instancer, *m_worldRule, cellBuilder, cellRule);
   
    m_worldBuilder->detach();
    
    typedef grd::LocalGridLookupRule<grd::LocalGrid<float>, InstancerTyp > LocalLookupRuleTyp;
    
    typedef grd::WorldGridLookupRule<WorldTyp, WorldCellTyp, LocalLookupRuleTyp > WorldLookupRuleTyp;
    WorldLookupRuleTyp *m_worldLookupRule = new WorldLookupRuleTyp;
    m_worldLookupRule->attach(m_worldGrid);
    m_worldLookupRule->setPrimitiveRule<InstancerTyp>(m_instancer);
    
}

int main(int argc, char *argv[])
{
    std::cout<<"\n test bvh ";
    testBvh();
    std::cout<<"\n passed test ";
      
    return 0;
}
