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
#include <h5/H2DDataset.h>
#include <math/miscfuncs.h>

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
	pql[2] = field.fineDistanceStorageSize();
	if(!hasNamedAttr(".pql") )
	    addIntAttr(".pql", 3);
	writeIntAttr(".pql", pql );
	
	const int coarsedL = field.coarseDistanceStorageSize();
	const int ncol = DivideUp(coarsedL, 1024);
	
	hdata::Select2DPart rect;
	rect.start[0] = 0;
	rect.start[1] = 0;
	rect.count[0] = ncol;
	rect.count[1] = 1024;
	
	H2dDataset<hdata::TChar, 1024, 32> coarsed(".coarse");
	coarsed.create(fObjectId);
	coarsed.write((char* )field.c_coarseDistanceValue(), &rect);
	coarsed.close();
	
	rect.count[0] = DivideUp(field.cellIndLength(), 256);
	rect.count[1] = 256;
	
	H2dDataset<hdata::TInt, 256, 32> cellid(".celli");
	cellid.create(fObjectId);
	cellid.write((char* )field.c_cellIndValue(), &rect);
	cellid.close();
	
	rect.count[0] = DivideUp(field.fineDistanceStorageSize(), 1024);
	rect.count[1] = 1024;
	
	H2dDataset<hdata::TChar, 1024, 32> fined(".fine");
	fined.create(fObjectId);
	fined.write((char* )field.c_fineDistanceValue(), &rect);
	fined.close();
    
    rect.count[0] = DivideUp(field.coarseNormalStorageSize(), 1024);
	rect.count[1] = 1024;
	
	H2dDataset<hdata::TChar, 1024, 32> coarseNml(".coarse_nml");
	coarseNml.create(fObjectId);
	coarseNml.write((char* )field.c_coarseNormalValue(), &rect);
	coarseNml.close();
    
    rect.count[0] = DivideUp(field.fineNormalStorageSize(), 1024);
	rect.count[1] = 1024;
	
	H2dDataset<hdata::TChar, 1024, 32> fineNml(".fine_nml");
	fineNml.create(fObjectId);
	fineNml.write((char* )field.c_fineNormalValue(), &rect);
	fineNml.close();
	
	std::cout<<"\n HSsdf save "<<pathToObject();
    field.verbose();
	
	return true;
}

template<typename T>
bool HSsdf::load(T& field)
{
	float orih[4];
	readFloatAttr(".coord", orih);
	field.setOriginCellSize(orih);
    
    float b[6];
    readFloatAttr(".aabb", b);
    field.setAabb(b);
	
	int pql[3];
	readIntAttr(".pql", pql );
	field.create(pql[0], pql[1], pql[2]);
	
	const int coarsedL = field.coarseDistanceStorageSize();
	const int ncol = DivideUp(coarsedL, 1024);
	
	hdata::Select2DPart rect;
	rect.start[0] = 0;
	rect.start[1] = 0;
	rect.count[0] = ncol;
	rect.count[1] = 1024;
	
	H2dDataset<hdata::TChar, 1024, 32> coarsed(".coarse");
	coarsed.open(fObjectId);
	coarsed.read((char* )field.coarseDistanceValue(), &rect);
	coarsed.close();
	
	rect.count[0] = DivideUp(field.cellIndLength(), 256);
	rect.count[1] = 256;
	
	H2dDataset<hdata::TInt, 256, 32> cellid(".celli");
	cellid.open(fObjectId);
	cellid.read((char* )field.cellIndValue(), &rect);
	cellid.close();
	
	rect.count[0] = DivideUp(field.fineDistanceStorageSize(), 1024);
	rect.count[1] = 1024;
	
	H2dDataset<hdata::TChar, 1024, 32> fined(".fine");
	fined.open(fObjectId);
	fined.read((char* )field.fineDistanceValue(), &rect);
	fined.close();
    
    rect.count[0] = DivideUp(field.coarseNormalStorageSize(), 1024);
	rect.count[1] = 1024;
	
	H2dDataset<hdata::TChar, 1024, 32> coarseNml(".coarse_nml");
	coarseNml.open(fObjectId);
	coarseNml.read((char* )field.coarseNormalValue(), &rect);
	coarseNml.close();
    
    rect.count[0] = DivideUp(field.fineNormalStorageSize(), 1024);
	rect.count[1] = 1024;
	
	H2dDataset<hdata::TChar, 1024, 32> fineNml(".fine_nml");
	fineNml.open(fObjectId);
	fineNml.read((char* )field.fineNormalValue(), &rect);
	fineNml.close();
	
	std::cout<<"\n HSsdf load "<<pathToObject();
    field.verbose();

	return true;
}

}

#endif
