/*
 *  HSurfaceGeodesicSample.h
 *  gorse
 *
 *  2019/7/15
 */
 
#ifndef ALO_H_SURFACE_GEODESIC_SAMPLE_H
#define ALO_H_SURFACE_GEODESIC_SAMPLE_H

#include <h5/V1HBase.h>
#include <math/miscfuncs.h>
#include <h5/HOocArray.h>

namespace alo {
   
class HSurfaceGeodesicSample : public ver1::HBase {

    typedef HOocArray<hdata::TChar, 64, 32> GeodData;
    
public:

	HSurfaceGeodesicSample(const std::string & name);
	
	virtual bool verifyType();
	
	template<typename T>
	bool save(const T& sampleArray);
	
	template<typename T>
	bool load(T* y);
	
protected:

private:

};

template<typename T>
bool HSurfaceGeodesicSample::save(const T& sampleArray)
{
    const int n = sampleArray.size();
    if(!hasNamedAttr(".is_surf_geod_sample") )
	    addIntAttr(".is_surf_geod_sample", 1);
	writeIntAttr(".is_surf_geod_sample", &n );
    
    bool stat;
    GeodData *smpd = createDataStorage<GeodData>(".smps", true, stat);
    if(!stat) {
    	std::cout << "\n ERROR HSurfaceGeodesicSample cannot create ooc storage smp";
    	return false;
    }
    
    char transient[4096];
    const int nw = n / 64;
    for(int i=0;i<nw;++i) {
        const int offset = i * 64;
        for(int j=0;j<64;++j) {
            if(offset + j > n-1) break;
            memcpy(&transient[j * 64], &sampleArray[offset + j], 64);
        }
        
        smpd->writeColumns(transient, offset, 64);
    }
    
    delete smpd;
    std::cout << "\n HSurfaceGeodesicSample saved n sample " << n;
    return true;
}

template<typename T>
bool HSurfaceGeodesicSample::load(T* y)
{
	int n = 0;
	readIntAttr(".is_surf_geod_sample", &n );
	y->create(n);
	
	bool stat;
    GeodData *smpd = openDataStorage<GeodData>(".smps", stat);
    if(!stat) {
    	std::cout << "\n ERROR HSurfaceGeodesicSample cannot open ooc storage smp";
    	return false;
    }
	
	smpd->readColumns((char *)y->samples(), 0, n);
	delete smpd;
	std::cout<<"\n HSurfaceGeodesicSample load "<<pathToObject();
	return true;
}
   
}

#endif
