/*
 *  TrunkOps.h
 *  vachellia
 *
 *  2019/7/15
 */

#ifndef VCHL_TRUNK_OPS_H
#define VCHL_TRUNK_OPS_H

#include "VoxelOps.h"

namespace alo {
	
struct SurfaceGeodesicSample;
	
namespace smp {
template<typename T>
class SampleFilter;
}

class TrunkOps : public VoxelOps {
	
	typedef smp::SampleFilter<SurfaceGeodesicSample> SampleCacheTyp;
	SampleCacheTyp *m_geodesicSamples;

	static AFileDlgProfile SReadTrunkProfile;
	
public:
	enum { Type = 703428 };

    TrunkOps();
    virtual ~TrunkOps();

    virtual std::string opsName() const override;
	virtual AFileDlgProfile *readFileProfileR() const override;
	virtual QString getShortDescription() const override;
	
protected:
    
	virtual bool loadSsdfCache(const std::string &fileName) override;
	
private:

};

} /// end of namespace alo

#endif

