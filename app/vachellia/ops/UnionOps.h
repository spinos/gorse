/*
 *  UnionOps.h
 *  vachellia
 *
 */

#ifndef VCHL_UNION_OPS_H
#define VCHL_UNION_OPS_H

#include "TransformOps.h"

namespace alo {

class UnionOps : public TransformOps {

public:
	enum { Type = 800001 };

    UnionOps();
    virtual ~UnionOps();

    virtual std::string opsName() const override;

    virtual void addRenderableTo(RenderableScene *scene) override;

    virtual void update() override;
    
    virtual bool intersectRay(const Ray& aray, IntersectResult& result) override;

protected:
    
private:
    
};

}

#endif

