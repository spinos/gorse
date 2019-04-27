/*
 *  VoxelOps.h
 *  vachellia
 *
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
}

class VoxelOps : public TransformOps {

    typedef sdf::SsdfLookupRule<sdf::SsdField> LookupRuleTyp;

    struct FieldLookupRulePair {
        sdf::SsdField *_field;
        LookupRuleTyp *_rule;
    };

    FieldLookupRulePair *m_pairs;
    int m_numPairs;

    static AFileDlgProfile SReadProfile;
    //sdf::SsdField *m_field;
    //sdf::SsdfLookupRule<sdf::SsdField> *m_rule;
    std::string m_cachePath;
    ElementVector<RenderableOps> m_outOps;
    int m_maxNStep;

public:
	enum { Type = 703427 };

    VoxelOps();
    virtual ~VoxelOps();

    virtual std::string opsName() const override;

    virtual void addRenderableTo(RenderableScene *scene) override;

    virtual void update() override;
    virtual AFileDlgProfile *readFileProfileR () const override;
    
    virtual bool intersectRay(const Ray& aray, IntersectResult& result) override;

    virtual void connectTo(GlyphOps *another, const std::string &portName) override;
    virtual void disconnectFrom(GlyphOps *another, const std::string &portName) override;
    
    virtual float mapDistance(const float *q) const override;
    virtual Vector3F mapNormal(const float *q) const override;

protected:
    
private:
    bool loadCache(const std::string &fileName);
    void clearAllPairs();
    void updateAllPairsAabb();
    void setAllRelativeBoundaryOffset(float x);
    float mapLocalDistanceTo(const float *q, int &objI) const;

};

}

#endif

