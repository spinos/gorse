/*
 *  VoxelOps.h
 *  vachellia
 *
 *  2019/5/5
 */

#ifndef VCHL_VOXEL_OPS_H
#define VCHL_VOXEL_OPS_H

#include "TransformOps.h"
#include <math/ElementVector.h>

namespace alo {

namespace sdf {
class SsdField;
template<typename T>
class SsdfLookupRule;

template<typename T>
class MultiLookupRule;

}

namespace grd {

template<typename T, typename Tp>
class LocalGridLookupRule;

template<typename T>
class LocalGrid;
}

class VoxelOps : public TransformOps {

    typedef sdf::MultiLookupRule<sdf::SsdField> PrimitiveLookupRule;
    PrimitiveLookupRule *m_primitiveLookup;

    typedef grd::LocalGrid<float> GridTyp;

    GridTyp *m_grid;

    typedef grd::LocalGridLookupRule<GridTyp, PrimitiveLookupRule > GridLookupRuleTyp;

    GridLookupRuleTyp *m_gridRule;

    static AFileDlgProfile SReadProfile;
    
    std::string m_cachePath;
    ElementVector<GlyphConnection> m_outOps;

public:
	enum { Type = 703427 };

    VoxelOps();
    virtual ~VoxelOps();

    virtual std::string opsName() const override;

    virtual void addRenderableTo(RenderableScene *scene) override;

    virtual bool hasInstance() const override;

    virtual void update() override;
    virtual AFileDlgProfile *readFileProfileR () const override;
    
    virtual bool intersectRay(IntersectResult& result) const override;

    virtual void connectTo(GlyphOps *another, const std::string &portName, GlyphConnection *line) override;
    virtual void disconnectFrom(GlyphOps *another, const std::string &portName, GlyphConnection *line) override;
    
    virtual float mapDistance(const float *q) const override;
    virtual Vector3F mapNormal(const float *q) const override;
    virtual float mapLocalDistance(const float *q) const override;
    virtual QString getShortDescription() const override;
    virtual void genSamples(sds::SpaceFillingVector<grd::PointSample> &samples) const override;
    
protected:
    
private:
    bool loadCache(const std::string &fileName);
    
};

} /// end of namespace alo

#endif

