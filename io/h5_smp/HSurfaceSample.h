/*
 *  HSurfaceSample.h
 *  gorse
 *
 *  2019/8/12
 */
 
#ifndef ALO_H_SURFACE_SAMPLE_H
#define ALO_H_SURFACE_SAMPLE_H

#include <h5/V1HBase.h>
#include <math/miscfuncs.h>
#include <h5/HOocArray.h>
#include <math/roundi.h>

namespace alo {
   
class HSurfaceSample : public ver1::HBase {

    typedef HOocArray<hdata::TChar, 40, 32> SurfData;
    
public:

	HSurfaceSample(const std::string & name);
	
	virtual bool verifyType();
	
	template<typename T>
	bool save(const T& sampleArray);
	
	template<typename T>
	bool load(T* y);
	
protected:

private:

};

template<typename T>
bool HSurfaceSample::save(const T& sampleArray)
{
    int n = sampleArray.size();
    if(!hasNamedAttr(".is_surf_sample") )
	    addIntAttr(".is_surf_sample", 1);
	writeIntAttr(".is_surf_sample", &n );
    
    bool stat;
    SurfData *smpd = createDataStorage<SurfData>(".smps", true, stat);
    if(!stat) {
    	std::cout << "\n ERROR HSurfaceSample cannot create ooc storage smp";
    	return false;
    }
    
    char transient[2560];
    const int nw = Round64(n) / 64;
    for(int i=0;i<nw;++i) {
        const int offset = i * 64;
        for(int j=0;j<64;++j) {
            if(offset + j > n-1) break;
            memcpy(&transient[j * 40], &sampleArray[offset + j], 40);
        }
        
        smpd->writeColumns(transient, offset, 64);
    }
    
    delete smpd;
    std::cout << "\n HSurfaceSample saved n sample " << n;
    return true;
}

template<typename T>
bool HSurfaceSample::load(T* y)
{
	int n = 0;
	readIntAttr(".is_surf_sample", &n );
	y->create(n);
	
	bool stat;
    SurfData *smpd = openDataStorage<SurfData>(".smps", stat);
    if(!stat) {
    	std::cout << "\n ERROR HSurfaceSample cannot open ooc storage smp";
    	return false;
    }
	
	smpd->readColumns((char *)y->samples(), 0, n);
	delete smpd;
	std::cout<<"\n HSurfaceSample load "<<pathToObject() <<" n sample "<< n;
	return true;
}
   
}

#endif
