/*
 *  V1HBase.cpp
 *  aloe
 *
 *
 */

#include "V1HBase.h"
#include "HIntAttribute.h"
#include "HFloatAttribute.h"
#include "HStringAttribute.h"
#include <sstream>

namespace alo {
    
namespace ver1 {

HBase::HBase(const std::string & path) : HGroup(path) 
{
	if(fObjectPath == "/") {
		open();
		return;
	}
	
	if(!HObject::FileIO.checkExist(fObjectPath))
		create();
	else 
		open();
}

HBase::~HBase() {}

void HBase::addIntAttr(const char * attrName, int dim)
{
	HIntAttribute nvAttr(attrName);
	nvAttr.create(dim, fObjectId);
	nvAttr.close();
}

void HBase::addFloatAttr(const char * attrName, int dim)
{
	HFloatAttribute attr(attrName);
	attr.create(dim, fObjectId);
	attr.close();
}

void HBase::addStringAttr(const char * attrName, int dim)
{
	HStringAttribute attr(attrName);
	attr.create(dim, fObjectId);
	attr.close();
}

void HBase::addVLStringAttr(const char * attrName)
{
	HVLStringAttribute attr(attrName);
	attr.create(1, fObjectId);
	attr.close();
}

void HBase::writeIntAttr(const char * attrName, int *value)
{
	HIntAttribute nvAttr(attrName);
	nvAttr.open(fObjectId);
	if(!nvAttr.write(value)) std::cout<<attrName<<" write failed";
	nvAttr.close();
}

void HBase::writeFloatAttr(const char * attrName, float *value)
{
	HFloatAttribute attr(attrName);
	attr.open(fObjectId);
	if(!attr.write(value)) std::cout<<attrName<<" write failed";
	attr.close();
}

void HBase::writeStringAttr(const char * attrName, const std::string & value)
{
	HStringAttribute attr(attrName);
	attr.open(fObjectId);
	if(!attr.write(value)) std::cout<<attrName<<" write failed";
	attr.close();
}

void HBase::writeVLStringAttr(const char * attrName, const std::string & value)
{
	HVLStringAttribute attr(attrName);
	attr.open(fObjectId);
	if(!attr.write(value)) std::cout<<attrName<<" write failed";
	attr.close();
}

char HBase::readIntAttr(const char * dataName, int *value)
{	
	HIntAttribute nvAttr(dataName);
	if(!nvAttr.open(fObjectId)) {
		std::cout<<dataName<<" open failed";
		return 0;
	}
	
	if(!nvAttr.read(value)) {
		std::cout<<dataName<<" read failed";
		return 0;
	}

	nvAttr.close();
	
	return 1;
}

char HBase::readFloatAttr(const char * attrName, float *value)
{
	HFloatAttribute attr(attrName);
	if(!attr.open(fObjectId)) {
		std::cout<<attrName<<" open failed";
		return 0;
	}
	
	if(!attr.read(value)) {
		std::cout<<attrName<<" read failed";
		return 0;
	}

	attr.close();
	
	return 1;
}

char HBase::readStringAttr(const char * attrName, std::string & value)
{
	HStringAttribute attr(attrName);
	if(!attr.open(fObjectId)) {
		std::cout<<attrName<<" open failed";
		return 0;
	}
	
	if(!attr.read(value)) {
		std::cout<<attrName<<" read failed";
		return 0;
	}

	attr.close();
	
	return 1;
}

char HBase::readVLStringAttr(const char * attrName, std::string & value)
{
	HVLStringAttribute attr(attrName);
	if(!attr.open(fObjectId)) {
		std::cout<<attrName<<" open failed";
		return 0;
	}
	
	if(!attr.read(value)) {
		std::cout<<attrName<<" read failed";
		return 0;
	}

	attr.close();
	
	return 1;
}

bool HBase::hasNamedAttr(const char * attrName)
{ return H5Aexists(fObjectId, attrName) > 0; }

std::string HBase::getAttrName(hid_t attrId)
{
	char memb_name[1024];
	H5Aget_name(attrId, (size_t)1024, 
		memb_name );
	std::stringstream sst;
	sst.str("");
	sst<<memb_name;
	return sst.str();
}

bool HBase::hasNamedChild(const char * childName)
{
	hsize_t nobj = 0;
	H5Gget_num_objs(fObjectId, &nobj);
	//std::cout<<"\n"<<fObjectPath<<" has "<<nobj<<"objs\n";
	hsize_t i;
	for(i = 0; i < nobj; i++) {
		//std::cout<<getChildName(i)<<"\n";
		if(getChildName(i) == childName) {
			//std::cout<<"found "<<childName;
			return true;
		}
	}
	return false;
}

std::string HBase::getChildName(hsize_t i)
{
	char memb_name[1024];
	H5Gget_objname_by_idx(fObjectId, (hsize_t)i, 
		memb_name, (size_t)1024 );
	std::stringstream sst;
	sst.str("");
	sst<<memb_name;
	return sst.str();
}

bool HBase::isChildGroup(hsize_t i)
{
	int otype = H5Gget_objtype_by_idx(fObjectId, (size_t)i);
	return otype == H5G_GROUP;
}

bool HBase::isChildData(hsize_t i)
{
	int otype = H5Gget_objtype_by_idx(fObjectId, (size_t)i);
	return otype == H5G_DATASET;
}

char HBase::hasNamedData(const char * dataName)
{
	hsize_t nobj;
	H5Gget_num_objs(fObjectId, &nobj);
	//std::cout<<"\n"<<fObjectPath<<" has "<<nobj<<"objs\n";
	hsize_t i;
	for(i = 0; i < nobj; i++) {
		//std::cout<<getChildName(i)<<"\n";
		if(isChildData(i)) {
            if(getChildName(i) == dataName) {
                //std::cout<<"found "<<dataName<<"\n";;
                return 1;
            }
        }
	}
	return 0;
}

char HBase::discardNamedAttr(const char * path)
{
    if(H5Adelete(fObjectId, path) < 0) return 0;
    return 1;
}

int HBase::numChildren()
{
	hsize_t nobj = 0;
	H5Gget_num_objs(fObjectId, &nobj);
	return (int)nobj;
}

int HBase::numAttrs()
{
    H5O_info_t oinfo;
    H5Oget_info(fObjectId, &oinfo);
    return oinfo.num_attrs;
}

int HBase::numDatas()
{
	int res = 0;
	hsize_t nobj;
	H5Gget_num_objs(fObjectId, &nobj);
	hsize_t i;
	for(i = 0; i < nobj; i++) {
		if(isChildData(i))
            res++;
	}
	return res;
}

std::string HBase::childPath(const std::string & name) const
{
	std::stringstream sst;
	sst.str("");
	sst<<fObjectPath<<"/"<<name;
	return sst.str();
}

std::string HBase::childPath(int i)
{
	std::stringstream sst;
	sst.str("");
	sst<<fObjectPath<<"/"<<getChildName(i);
	return sst.str();
}

bool HBase::hasNamedAttrIntVal(const std::string & attrName,
								int attrVal)
{
	if(!hasNamedAttr(attrName.c_str() ) ) return false;
	int v = -1;
	if(!readIntAttr(attrName.c_str(), &v) ) return false;
	return (v == attrVal);
}

bool HBase::save()
{ return true; }

bool HBase::load()
{ return true; }

bool HBase::verifyType()
{ return true; }

}

}
