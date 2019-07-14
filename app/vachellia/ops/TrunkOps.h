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

class TrunkOps : public VoxelOps {


public:
	enum { Type = 703428 };

    TrunkOps();
    virtual ~TrunkOps();

    virtual std::string opsName() const override;

protected:
    
private:

};

} /// end of namespace alo

#endif

