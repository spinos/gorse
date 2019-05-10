/*
 *  TransformOps.h
 *  vachellia
 *
 *  2019/4/25
 */

#ifndef VCHL_TRANSFORM_OPS_H
#define VCHL_TRANSFORM_OPS_H

#include "RenderableOps.h"
#include <math/TransformComponent.h>

namespace alo {

class TransformOps : public RenderableOps, public TransformComponent {

public:

	TransformOps();
	virtual ~TransformOps();

	virtual void update() override;
	virtual bool intersectRay(const Ray& aray, IntersectResult& result) const override;

	virtual bool hasMenu() const override;
    virtual void getMenuItems(std::vector<std::pair<std::string, int > > &ks) const override;
    virtual void recvAction(int x) override;
    
    virtual void expandAabb(float *box) const override;

    virtual float mapDistance(const float *q) const override;
/// gradient of distance
/// q is local
    virtual Vector3F mapNormal(const float *q) const override;

protected:

private:

};

}

#endif
