/*
 *  VoxelOps.h
 *  vachellia
 *
 */

#ifndef VCHL_VOXEL_OPS_H
#define VCHL_VOXEL_OPS_H

#include "RenderableOps.h"

namespace alo {

namespace sdf {
class SsdField;
template<typename T>
class SsdfLookupRule;
}

class VoxelOps : public RenderableOps {
    
    static AFileDlgProfile SReadProfile;
    sdf::SsdField *m_field;
    sdf::SsdfLookupRule<sdf::SsdField> *m_rule;
    std::string m_cachePath;

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
    bool loadCache(const std::string &fileName);

};

}

#endif

