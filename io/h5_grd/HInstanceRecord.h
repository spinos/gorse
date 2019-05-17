/*
 *  HInstanceRecord.h
 *  gorse
 *
 *  2019/5/17
 */

#ifndef ALO_H5_INSTANCE_RECORD_H
#define ALO_H5_INSTANCE_RECORD_H

#include <h5/V1HBase.h>
#include <h5/HOocArray.h>

namespace alo {
    
namespace grd {
class InstanceRecord;
}
    
class HInstanceRecord : public ver1::HBase {

public:

	HInstanceRecord(const std::string &name);
	
	virtual bool verifyType();

	bool save(const grd::InstanceRecord &rec);

	bool load(grd::InstanceRecord &rec);
	
protected:

private:
    typedef HOocArray<hdata::TInt, 32, 32> IntData;
    typedef HOocArray<hdata::TFloat, 512, 128> MatData;
    
};
    
}

#endif
