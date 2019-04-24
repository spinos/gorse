/*
 *  UnionOps.h
 *  vachellia
 *
 */

#ifndef VCHL_UNION_OPS_H
#define VCHL_UNION_OPS_H

#include "TransformOps.h"
#include <math/ElementVector.h>

namespace alo {

class UnionOps : public TransformOps {

	ElementVector<RenderableOps> m_inOps;

public:
	enum { Type = 800001 };

    UnionOps();
    virtual ~UnionOps();

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

