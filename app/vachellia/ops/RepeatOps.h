/*
 *  RepeatOps.h
 *  vachellia
 *
 *  2019/5/4
 */

#ifndef VCHL_REPEAT_OPS_H
#define VCHL_REPEAT_OPS_H

#include "TransformOps.h"
#include <math/ElementVector.h>
#include <sdb/Types.h>

namespace alo {

namespace grd {
class IndexGrid;

template<typename T>
struct GridInCell;

template<typename T, typename Tc>
class WorldGrid;

template<typename T, typename Tc, typename Tf>
class WorldGridBuildRule;

template<typename T, typename Tc>
class WorldGridBuilder;

template<typename T, typename Tc, typename Ti>
class WorldGridLookupRule;

template<typename T1, typename T2>
class ObjectInstancer;

struct TestInstance;

struct BoxObject;

}

namespace sds {
struct FZOrderCurve;
}

class RepeatOps : public TransformOps {

	ElementVector<RenderableOps> m_inOps;

    typedef grd::GridInCell<grd::IndexGrid> WorldCellTyp;

    typedef grd::WorldGrid<int, WorldCellTyp > WorldTyp;
    WorldTyp *m_worldGrid;

    typedef grd::WorldGridBuildRule<int, WorldCellTyp, sds::FZOrderCurve > WorldRuleTyp;
    WorldRuleTyp *m_worldRule;

    typedef grd::WorldGridBuilder<int, WorldCellTyp > WorldBuilderTyp;
    WorldBuilderTyp *m_worldBuilder;

    typedef grd::ObjectInstancer<grd::TestInstance, grd::BoxObject> InstancerTyp;
    InstancerTyp *m_instancer;

    typedef grd::WorldGridLookupRule<WorldTyp, WorldCellTyp, InstancerTyp > WorldLookupRuleTyp;
    WorldLookupRuleTyp *m_worldLookupRule;

public:
	enum { Type = 800002 };

    RepeatOps();
    virtual ~RepeatOps();

    virtual std::string opsName() const override;

    virtual void addRenderableTo(RenderableScene *scene) override;

    virtual void update() override;
    
    virtual bool intersectRay(const Ray& aray, IntersectResult& result) override;

    virtual bool canConnectTo(GlyphOps *another, const std::string &portName) const override;
    virtual void connectTo(GlyphOps *another, const std::string &portName) override;
    virtual void disconnectFrom(GlyphOps *another, const std::string &portName) override;

    virtual float mapDistance(const float *q) const override;
    virtual Vector3F mapNormal(const float *q) const override;
    
protected:
    
private:
    
};

}

#endif

