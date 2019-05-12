/*
 *  ConeOps.h
 *  vachellia
 *
 *  2019/5/11
 */

#ifndef VCHL_CONE_OPS_H
#define VCHL_CONE_OPS_H

#include "TransformOps.h"
#include <math/ElementVector.h>

namespace alo {

class ConeOps : public TransformOps {
    
    ElementVector<GlyphConnection> m_outOps;
    float m_height;
    float m_radius;
    float m_lcrc;

public:
	enum { Type = 299097 };

    ConeOps();
    virtual ~ConeOps();

    virtual std::string opsName() const override;

    virtual void addRenderableTo(RenderableScene *scene) override;

    virtual bool hasInstance() const override;

    virtual void update() override;

    virtual bool intersectRay(IntersectResult& result) const override;
    
    virtual void connectTo(GlyphOps *another, const std::string &portName, GlyphConnection *line) override;
    virtual void disconnectFrom(GlyphOps *another, const std::string &portName, GlyphConnection *line) override;

    virtual float mapDistance(const float *q) const override;
    virtual Vector3F mapNormal(const float *q) const override;
    virtual float mapLocalDistance(const float *q) const override;
    virtual void genSamples(sds::SpaceFillingVector<grd::PointSample> &samples) const override;
    
protected:
    
private:
    
};

}

#endif

