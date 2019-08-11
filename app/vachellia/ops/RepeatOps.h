/*
 *  RepeatOps.h
 *  vachellia
 *
 *  2019/7/18
 */

#ifndef VCHL_REPEAT_OPS_H
#define VCHL_REPEAT_OPS_H

#include "TransformOps.h"
#include "InstancerBase.h"

namespace alo {

class RepeatOps : public TransformOps, public InstancerBase {

    std::string m_instanceFilePath;
    static AFileDlgProfile SReadProfile;

public:

	enum { Type = 800002 };

    RepeatOps();
    virtual ~RepeatOps();

    virtual std::string opsName() const override;

    virtual void addRenderableTo(RenderableScene *scene) override;

    virtual bool hasInstance() const override;
    virtual bool hasGeodesicSamples() const override;
    virtual bool hasSurfaceSamples() const override;
    virtual bool hasEnable() const override;
    virtual void update() override;
    virtual void setActivated(bool x) override;
    
    virtual bool intersectRay(IntersectResult& result) const override;

    virtual bool canConnectTo(GlyphOps *another, const std::string &portName) const override;
    virtual void connectTo(GlyphOps *another, const std::string &portName, GlyphConnection *line) override;
    virtual void disconnectFrom(GlyphOps *another, const std::string &portName, GlyphConnection *line) override;
    virtual QString getShortDescription() const override;
    virtual AFileDlgProfile *readFileProfileR() const override;
    virtual bool getActivatedState() const override;

protected:
    
private:
    
    bool loadInstanceFile(const std::string &fileName);
    
};

}

#endif

