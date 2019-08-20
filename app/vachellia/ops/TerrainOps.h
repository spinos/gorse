/*
 *  TerrainOps.h
 *  vachellia
 *
 *  2019/7/29
 */

#ifndef VCHL_TERRAIN_OPS_H
#define VCHL_TERRAIN_OPS_H

#include "VoxelOps.h"

namespace alo {

class TerrainOps : public VoxelOps {
	
	typedef smp::SampleFilter<SurfaceSample> SampleCacheTyp;
	SampleCacheTyp *m_surfaceSamples;

	static AFileDlgProfile SReadTrunkProfile;
	
public:
	enum { Type = 703429 };

    TerrainOps();
    virtual ~TerrainOps();

    virtual std::string opsName() const override;
    virtual bool hasInstance() const override;
    virtual bool hasSurfaceSamples() const override;
	virtual AFileDlgProfile *readFileProfileR() const override;
	virtual QString getShortDescription() const override;
	virtual const smp::SampleFilter<SurfaceSample> *getSurfaceSamples() const override;
    
protected:
    
	virtual bool loadSsdfCache(const std::string &fileName) override;
	
private:

};

} /// end of namespace alo

#endif
