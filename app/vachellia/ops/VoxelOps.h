/*
 *  VoxelOps.h
 *  vachellia
 *
 */

#ifndef VCHL_VOXEL_OPS_H
#define VCHL_VOXEL_OPS_H

#include "RenderableOps.h"

namespace alo {

class VoxelOps : public RenderableOps {
    
    static AFileDlgProfile SReadProfile;
    
public:
	enum { Type = 703427 };

    VoxelOps();
    virtual ~VoxelOps();

    virtual void addRenderableTo(RenderableScene *scene) override;

    virtual void update() override;
    virtual AFileDlgProfile *readFileProfileR () const override;
    
    virtual bool intersectRay(const Ray& aray, IntersectResult& result) override;

protected:
    
private:
    

};

}

#endif

