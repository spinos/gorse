/*
 *  V1HBase.h
 *  aloe
 *
 *  2019/4/29
 */

#ifndef ALO_V1_H_BASE_H
#define ALO_V1_H_BASE_H

#include "HGroup.h"

#include <string>
#include <iostream>
#include <vector>

namespace alo {
    
namespace ver1 {

class HBase : public HGroup {
public:
	HBase(const std::string & path);
	virtual ~HBase();
	
	void addIntAttr(const char * attrName, int dim = 1);
	void addFloatAttr(const char * attrName, int dim = 1);
	void addStringAttr(const char * attrName, int dim);
	void addVLStringAttr(const char * attrName);
	
	void writeIntAttr(const char * attrName, int *value);
	void writeFloatAttr(const char * attrName, float *value);
	void writeStringAttr(const char * attrName, const std::string & value);
	void writeVLStringAttr(const char * attrName, const std::string & value);
	
	char readIntAttr(const char * attrName, int *value);
	char readFloatAttr(const char * attrName, float *value);
	char readStringAttr(const char * attrName, std::string & value);
	char readVLStringAttr(const char * attrName, std::string & value);
	
	bool hasNamedAttr(const char * attrName);
	std::string getAttrName(hid_t attrId);
	
	bool hasNamedChild(const char * childName);
	std::string getChildName(hsize_t i);
	bool isChildGroup(hsize_t i);
	bool isChildData(hsize_t i);
	
	char hasNamedData(const char * dataName);
	char discardNamedAttr(const char * path);
	
	bool hasNamedAttrIntVal(const std::string & attrName,
							int attrVal);
	
	int numChildren();
	int numAttrs();
	int numDatas();
	
	std::string childPath(const std::string & name) const;
	std::string childPath(int i);
	
	virtual bool save();
	virtual bool load();
    virtual bool verifyType();

	template<typename T>
	void lsTypedChild(std::vector<std::string> & names) {
		int nc = numChildren();
		int i = 0;
		for(;i<nc;i++) {
			if(isChildGroup(i)) {
				T gc(childPath(i));
				if(gc.verifyType()) names.push_back(childPath(i));
				gc.close();
			}
		}
	}
	
	template<typename T>
	void lsTypedChildWithIntAttrVal(std::vector<std::string> & names,
									const std::string & attrName,
									int attrVal ) {
		int nc = numChildren();
		int i = 0;
		for(;i<nc;i++) {
			if(isChildGroup(i)) {
				T gc(childPath(i));
				if(gc.verifyType()) {
					if(gc.hasNamedAttrIntVal(attrName, attrVal ) )
						names.push_back(childPath(i));
				}
				gc.close();
			}
		}
	}
	
	template<typename T>
	bool hasTypedChildWithIntAttrVal(const std::string & name,
									const std::string & attrName,
									int attrVal ) 
	{
		bool stat = false;
		if(hasNamedChild(name.c_str() ) ) {
			T gc(childPath(name ) );
			if(gc.verifyType() ) {
				if(gc.hasNamedAttrIntVal(attrName, attrVal ) )
					stat = true;
			}
			gc.close();
		}
		return stat;
	}
    
    void lsData(std::vector<std::string> & names) {
        int nc = numChildren();
		int i = 0;
		for(;i<nc;i++) {
			if(isChildData(i)) {
				names.push_back(childPath(i));
			}
		}
    }
	
	template<typename T>
	T * createDataStorage(const std::string & name, bool toClear, bool & stat)
	{
		T * d = new T(name);
		if(hasNamedData(name.c_str() ) ) {
			stat = d->openStorage(fObjectId, false);
			if(stat) stat = d->checkDataSpace();
		}
		else {
			if(d->createStorage(fObjectId)) {
				stat = true;
			}
			else {
				std::cout<<"\n HBase createDataStorage error";
				stat = false;
			}
		}
/// not a data set?
		//d->close();
		return d;
	}
	
	template<typename T>
	T * openDataStorage(const std::string & name, bool & stat)
	{
		if(!hasNamedData(name.c_str() ) ) {
			stat = false;
			return NULL;
		}
		T * d = new T(name);
		stat = d->openStorage(fObjectId);
		//d->close();		
		//std::cout<<"\n HBase:: open sto end"<<__LINE__<<__FILE__<<"\n";
    //std::cout.flush();
		return d;
	}
	
	template<typename T>
	void lsTypedChildHierarchy(std::vector<std::string>& log) 
	{
	    std::vector<std::string > tmNames;
        lsTypedChild<HBase >(tmNames);
        std::vector<std::string>::const_iterator it = tmNames.begin();
        
        for(;it!=tmNames.end();++it) {
            std::string nodeName = *it;
            
            HBase child(*it);
            
            child.lsTypedChildHierarchy<T>(log);
            
            child.close();
        }
        
        std::vector<std::string > tNames;
        lsTypedChild<T>(tNames);
        std::vector<std::string>::const_iterator ita = tNames.begin();
        
        for(;ita !=tNames.end();++ita) {
            std::string nodeName = *ita;
            log.push_back(nodeName);

        }
        
	}

	template<typename T>
	bool lsFirstTypedChild(std::string &name);
	
};

template<typename T>
bool HBase::lsFirstTypedChild(std::string &name) 
{
	bool found = false;
	int nc = numChildren();
	for(int i=0;i<nc;i++) {
		if(isChildGroup(i)) {
			T gc(childPath(i));
			if(gc.verifyType()) {
				name = childPath(i);
				found = true;
			}
			gc.close();
		}
		if(found) break;
	}
	return found;
}

} /// end of ver1

} /// end of alo
#endif       
