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
#include <math/SimpleBuffer.h>

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
	
	template<typename T>
	bool save(const T &rec);
	
protected:

private:
    typedef HOocArray<hdata::TInt, 64, 32> IntData;
    typedef HOocArray<hdata::TFloat, 1024, 256> MatData;
    
};

template<typename T>
bool HInstanceRecord::save(const T &rec)
{
	if(!hasNamedAttr(".is_instance_record") )
	    addIntAttr(".is_instance_record", 2);
    
    int mn[2];
    mn[0] = rec.numObjects();
    mn[1] = rec.numInstances();
    writeIntAttr(".is_instance_record", mn );
	
	float b[6];
    rec.getAabb(b);
    if(!hasNamedAttr(".bbox") )
	    addFloatAttr(".bbox", 6);
    writeFloatAttr(".bbox", b);
	
	bool stat;
	const int &n = rec.numInstances();

    IntData *indd = createDataStorage<IntData>(".obj_ind", true, stat);
    if(!stat) {
    	std::cout << "\n ERROR HInstanceRecord cannot create ooc storage obj_ind";
    	return false;
    }
    
	SimpleBuffer<int> itransient;
	itransient.createBuffer(4096);
/// 4096 int 64 per col 64 col per write
    const int niw = Round4096(n)>>12;
    for(int i=0;i<niw;++i) {
		const int offset = i<<12;
        for(int j=0;j<4096;++j) {
            if(offset + j > n-1) break;
			const T::InstanceTyp &inst = rec.c_instance(offset + j);
			itransient[j] = inst.objectId();
        }
        
        indd->writeColumns((char *)itransient.data(), i<<6, 64);
	}
    delete indd;
    
    MatData *tmd = createDataStorage<MatData>(".obj_tm", true, stat);
    if(!stat) {
    	std::cout << "\n ERROR HInstanceRecord cannot create ooc storage tm";
    	return false;
    }
    
	SimpleBuffer<Matrix44F> mtransient;
	mtransient.createBuffer(1024);
/// 1024 mat 64 per col 16 col per write
	const int nmw = Round1024(n)>>10;
    for(int i=0;i<nmw;++i) {
		const int offset = i<<10;
        for(int j=0;j<1024;++j) {
            if(offset + j > n-1) break;
			const T::InstanceTyp &inst = rec.c_instance(offset + j);
			mtransient[j] = inst.space();
        }
        
        tmd->writeColumns((char *)mtransient.data(), i<<4, 16);
	}
    
    delete tmd;
    
    std::cout<<"\n HInstanceRecord save "<<pathToObject()
			<<"\n write ind " << niw <<" write mat " << nmw;
	return true;
}
    
}

#endif
