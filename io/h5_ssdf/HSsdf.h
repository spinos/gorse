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
	
	int pql[3];
	pql[0] = field.P();
	pql[1] = field.Q();
	pql[2] = field.fineDistanceStorageSize();
	if(!hasNamedAttr(".pql") )
	    addIntAttr(".pql", 3);
	writeIntAttr(".pql", pql );
	
	const int coarsedL = field.coarsDistanceStorageSize();
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
	
	std::cout<<"\n HSsdf "<<pathToObject()<<" saved "<<pql[0]<<" "<<pql[1]
		<<" data "<<pql[2]<<" byte "<<std::endl;
	
	return true;
}

template<typename T>
bool HSsdf::load(T& field)
{
	float orih[4];
	readFloatAttr(".coord", orih);
	field.setOriginCellSize(orih);
	
	int pql[3];
	readIntAttr(".pql", pql );
	field.create(pql[0], pql[1], pql[2]);
	
	const int coarsedL = field.coarsDistanceStorageSize();
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
/*	
	const int d = 1<<pql[0];
	const int n = d*d*d;
	for(int i=0;i<n;++i) {
		std::cout<<" "<<field.c_cellIndValue()[i];
	}
	
	const int d1 = 1<<(pql[1]-pql[0]);
	const int d3 = (d1+1)*(d1+1)*(d1+1);
	for(int i=0;i<d3;++i) {
		std::cout<<" "<<field.c_fineDistanceValue()[i];
	}
*/	
	std::cout<<"\n HSsdf "<<pathToObject()<<" loaded "<<pql[0]<<" "<<pql[1]
		<<" data "<<pql[2]<<" byte "<<std::endl;

	return true;
}

}

#endif
