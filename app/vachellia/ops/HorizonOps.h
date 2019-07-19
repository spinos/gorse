/*
 *  HorizonOps.h
 *  vachellia
 *
 */

#ifndef VCHL_HORIZON_OPS_H
#define VCHL_HORIZON_OPS_H

#include "RenderableOps.h"

namespace alo {

class HorizonOps : public RenderableOps {
    
    Vector3F m_center;
    float m_planetRadius;

public:
	enum { Type = 298999 };

    HorizonOps();
    virtual ~HorizonOps();

    virtual std::string opsName() const override;

    virtual void addRenderableTo(RenderableScene *scene) override;

    virtual bool hasInstance() const override;
    virtual bool hasGeodesicSamples() const override;
    
    virtual void update() override;
    
    virtual bool intersectRay(IntersectResult& result) const override;
    
    virtual void expandAabb(float *box) const override;

    virtual float mapDistance(const float *q) const override;
    virtual Vector3F mapNormal(const float *q) const override;
    virtual float mapLocalDistance(const float *q) const override;
/// no samples generated
    virtual void genSamples(sds::SpaceFillingVector<grd::PointSample> &samples) const override;
    virtual QString getShortDescription() const override;
    
protected:
    
private:
    float radiusInMeters() const;

};

}

#endif

