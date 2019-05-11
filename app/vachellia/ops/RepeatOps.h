/*
 *  RepeatOps.h
 *  vachellia
 *
 *  2019/5/7
 */

#ifndef VCHL_REPEAT_OPS_H
#define VCHL_REPEAT_OPS_H

#include "TransformOps.h"
#include <math/ElementVector.h>
#include <sdb/Types.h>

namespace alo {

namespace grd {

template<typename T>
class LocalGrid;

template<typename T>
struct GridInCell;

template<typename T, typename Tc>
class WorldGrid;

template<typename T, typename Tf>
class WorldGridBuildRule;

template<typename T, typename Tc>
class WorldGridBuilder;

template<typename T, typename Tc, typename Ti>
class WorldGridLookupRule;

template<typename T1, typename T2>
class ObjectInstancer;

struct TestInstance;

//struct BoxObject;

template<typename T, typename Tp>
class LocalGridLookupRule;

}

namespace sds {
struct FZOrderCurve;
}

class RepeatOps : public TransformOps {

	ElementVector<RenderableOps> m_inOps;

    typedef grd::GridInCell<grd::LocalGrid<float> > WorldCellTyp;

    typedef grd::WorldGrid<int, WorldCellTyp > WorldTyp;
    WorldTyp *m_worldGrid;

    typedef grd::WorldGridBuildRule<int, sds::FZOrderCurve > WorldRuleTyp;
    WorldRuleTyp *m_worldRule;

    typedef grd::WorldGridBuilder<int, WorldCellTyp > WorldBuilderTyp;
    WorldBuilderTyp *m_worldBuilder;

    typedef grd::ObjectInstancer<grd::TestInstance, RenderableOps > InstancerTyp;
    InstancerTyp *m_instancer;

    typedef grd::LocalGridLookupRule<grd::LocalGrid<float>, InstancerTyp > LocalLookupRuleTyp;

    typedef grd::WorldGridLookupRule<WorldTyp, WorldCellTyp, LocalLookupRuleTyp > WorldLookupRuleTyp;
    WorldLookupRuleTyp *m_worldLookupRule;

public:
	enum { Type = 800002 };

    RepeatOps();
    virtual ~RepeatOps();

    virtual std::string opsName() const override;

    virtual void addRenderableTo(RenderableScene *scene) override;

    virtual bool hasInstance() const override;

    virtual void update() override;
    
    virtual bool intersectRay(IntersectResult& result) const override;

    virtual bool canConnectTo(GlyphOps *another, const std::string &portName) const override;
    virtual void connectTo(GlyphOps *another, const std::string &portName, GlyphConnection *line) override;
    virtual void disconnectFrom(GlyphOps *another, const std::string &portName, GlyphConnection *line) override;

    virtual float mapDistance(const float *q) const override;
    virtual Vector3F mapNormal(const float *q) const override;
    
protected:
    
private:
    void updateInstancer(bool isAppending);
    
};

}

#endif

