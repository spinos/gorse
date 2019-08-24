/*
 *  HInstanceRecord.cpp
 *  gorse
 *
 *  2019/5/17
 */
 
#include "HInstanceRecord.h"
#include <grd/InstanceRecord.h>

namespace alo {

HInstanceRecord::HInstanceRecord(const std::string &name) : ver1::HBase(name)
{}
	
bool HInstanceRecord::verifyType()
{ return hasNamedAttr(".is_instance_record"); }

bool HInstanceRecord::save(const grd::InstanceRecord &rec)
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
    
    IntData *indd = createDataStorage<IntData>(".obj_ind", true, stat);
    if(!stat) {
    	std::cout << "\n ERROR HInstanceRecord cannot create ooc storage obj_ind";
    	return false;
    }
    
    const int nc = Round64(rec.numInstances())>>6;
    indd->writeColumns((char *)rec.c_inds(), 0, nc);
    delete indd;
    
    MatData *tmd = createDataStorage<MatData>(".obj_tm", true, stat);
    if(!stat) {
    	std::cout << "\n ERROR HInstanceRecord cannot create ooc storage tm";
    	return false;
    }
    
    tmd->writeColumns((char *)rec.c_tms(), 0, nc);
    delete tmd;
    
    std::cout<<"\n HInstanceRecord save "<<pathToObject();
    rec.verbose();
    
    return true;
}

bool HInstanceRecord::load(grd::InstanceRecord &rec)
{
    int mn[2];
    readIntAttr(".is_instance_record", mn );
    rec.create(mn[0], mn[1]);
    
    float b[6];
    readFloatAttr(".bbox", b);
    rec.setAabb(b);
    
    bool stat;
    
    IntData *indd = openDataStorage<IntData>(".obj_ind", stat);
    if(!stat) {
        std::cout << "\n ERROR HInstanceRecord cannot open ooc storage obj_ind";
        return false;
    }
    
    const int nc = Round64(mn[1])>>6;
    
    indd->readColumns((char *)rec.inds(), 0, nc);
    delete indd;
    
    MatData *tmd = openDataStorage<MatData>(".obj_tm", stat);
    if(!stat) {
        std::cout << "\n ERROR HInstanceRecord cannot open ooc storage tm";
        return false;
    }

    tmd->readColumns((char *)rec.tms(), 0, nc);
    delete tmd;
    
    std::cout<<"\n HInstanceRecord load "<<pathToObject();
    rec.verbose();
    
    return true;
}

}
