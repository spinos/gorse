#include "HCoarseFineHistory.h"
#include <mesh/CoarseFineHistory.h>
#include <h5/HOocArray.h>
#include <math/roundi.h>

namespace alo {

HCoarseFineHistory::HCoarseFineHistory(const std::string & path) : ver1::HBase(path) 
{}

HCoarseFineHistory::~HCoarseFineHistory()
{}

bool HCoarseFineHistory::save(const CoarseFineHistory *his)
{
    if(!hasNamedAttr(".attr"))
		addIntAttr(".attr", 7);
	
	int attrs[7];
	attrs[0] = his->length();
	attrs[1] = his->vbegin();
	attrs[2] = his->vend();
	attrs[3] = his->ncoarse();
	attrs[4] = his->nfine();
	attrs[5] = his->fbegin();
	attrs[6] = his->fend();
	writeIntAttr(".attr", attrs);
	
	typedef HOocArray<hdata::TInt, 32, 32> IData;
	bool stat;
    IData *ind = createDataStorage<IData>(".his", true, stat);
    if(!stat) {
    	std::cout << "\n ERROR cannot create ooc storage .his";
    	return false;
    }
	
    int nc = Round32(attrs[0])>>5;
    
    ind->writeColumns((char *)his->c_value(), 0, nc);
    return true; 
}

}

