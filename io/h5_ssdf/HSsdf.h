/*
 *  HSsdf.h
 *  aloe
 *
 *  Created by jian zhang on 2/28/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_H_SSDF_H
#define ALO_H_SSDF_H

#include <h5/V1HBase.h>
#include <math/miscfuncs.h>
#include <h5/HOocArray.h>

namespace alo {

class HSsdf : public ver1::HBase {

public:

	HSsdf(const std::string & name);
	
	virtual bool verifyType();
	
	template<typename T>
	bool save(const T& field);
	
	template<typename T>
	bool load(T& field);
	
protected:

private:

};

template<typename T>
bool HSsdf::save(const T& field)
{
	if(!hasNamedAttr(".coord") )
	    addFloatAttr(".coord", 4);
	writeFloatAttr(".coord", (float* )field.originCellSize() );
    
    if(!hasNamedAttr(".aabb") )
	    addFloatAttr(".aabb", 6);
    float b[6];
    field.getAabb(b);
	writeFloatAttr(".aabb", b );
	
	int pql[3];
	pql[0] = field.P();
	pql[1] = field.Q();
	pql[2] = field.numFineValues();
	if(!hasNamedAttr(".pql") )
	    addIntAttr(".pql", 3);
	writeIntAttr(".pql", pql );

	typedef HOocArray<hdata::TFloat, 32, 32> FltData;
	bool stat;
    FltData *coarsd = createDataStorage<FltData>(".coarse", true, stat);
    if(!stat) {
    	std::cout << "\n ERROR HSsdf cannot create ooc storage coarse";
    	return false;
    }

    int nc = Round32(field.numValues())>>5;
    
    coarsd->writeColumns((char *)field.c_coarseDistanceValue(), 0, nc);
    delete coarsd;

    typedef HOocArray<hdata::TInt, 32, 32> IntData;
    IntData *celld = createDataStorage<IntData>(".celli", true, stat);
    if(!stat) {
    	std::cout << "\n ERROR HSsdf cannot create ooc storage celli";
    	return false;
    }

    nc = Round32(field.numCells())>>5;
    
    celld->writeColumns((char *)field.c_cellIndValue(), 0, nc);
    delete celld;

    FltData *fined = createDataStorage<FltData>(".fine", true, stat);
    if(!stat) {
    	std::cout << "\n ERROR HSsdf cannot create ooc storage fine";
    	return false;
    }

    nc = Round32(field.numFineValues())>>5;
    
    fined->writeColumns((char *)field.c_fineDistanceValue(), 0, nc);
    delete fined;

    FltData *coarseNmld = createDataStorage<FltData>(".coarse_nml", true, stat);
    if(!stat) {
    	std::cout << "\n ERROR HSsdf cannot create ooc storage coarse_nml";
    	return false;
    }

    nc = Round32(field.numValues()*3)>>5;
    
    coarseNmld->writeColumns((char *)field.c_coarseNormalValue(), 0, nc);
    delete coarseNmld;

    FltData *fineNmld = createDataStorage<FltData>(".fine_nml", true, stat);
    if(!stat) {
    	std::cout << "\n ERROR HSsdf cannot create ooc storage fine_nml";
    	return false;
    }

    nc = Round32(field.numFineValues()*3)>>5;
    
    fineNmld->writeColumns((char *)field.c_fineNormalValue(), 0, nc);
    delete fineNmld;
	
	std::cout<<"\n HSsdf save "<<pathToObject();
    field.verbose();
	
	return true;
}

template<typename T>
bool HSsdf::load(T& field)
{
	int pql[3];
	readIntAttr(".pql", pql );
	field.create(pql[0], pql[1], pql[2]);

	float orih[4];
	readFloatAttr(".coord", orih);
	field.setOriginCellSize(orih);
    
    float b[6];
    readFloatAttr(".aabb", b);
    field.setAabb(b);

    typedef HOocArray<hdata::TFloat, 32, 32> FltData;
    bool stat;
    FltData *coarsed = openDataStorage<FltData>(".coarse", stat);
    if(!stat) {
        std::cout << "\n ERROR HSsdf cannot open ooc storage coarse";
        return false;
    }

    int nc = Round32(field.numValues())>>5;
    
    coarsed->readColumns((char *)field.coarseDistanceValue(), 0, nc);
    delete coarsed;

    typedef HOocArray<hdata::TInt, 32, 32> IntData;
    IntData *celld = openDataStorage<IntData>(".celli", stat);
    if(!stat) {
        std::cout << "\n ERROR HSsdf cannot open ooc storage celli";
        return false;
    }

    nc = Round32(field.numCells())>>5;
    
    celld->readColumns((char *)field.cellIndValue(), 0, nc);
    delete celld;

    FltData *fined = openDataStorage<FltData>(".fine", stat);
    if(!stat) {
        std::cout << "\n ERROR HSsdf cannot open ooc storage fine";
        return false;
    }

    nc = Round32(field.numFineValues())>>5;
    
    fined->readColumns((char *)field.fineDistanceValue(), 0, nc);
    delete fined;

    FltData *coarseNmld = openDataStorage<FltData>(".coarse_nml", stat);
    if(!stat) {
        std::cout << "\n ERROR HSsdf cannot open ooc storage coarse_nml";
        return false;
    }

    nc = Round32(field.numValues()*3)>>5;
    
    coarseNmld->readColumns((char *)field.coarseNormalValue(), 0, nc);
    delete coarseNmld;

    FltData *fineNmld = openDataStorage<FltData>(".fine_nml", stat);
    if(!stat) {
        std::cout << "\n ERROR HSsdf cannot open ooc storage fine_nml";
        return false;
    }

    nc = Round32(field.numFineValues()*3)>>5;
    
    fineNmld->readColumns((char *)field.fineNormalValue(), 0, nc);
    delete fineNmld;
	
	std::cout<<"\n HSsdf load "<<pathToObject();
    field.verbose();

	return true;
}

}

#endif
