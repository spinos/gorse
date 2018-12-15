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
	
	int *attrs = (int *)&his->desc();
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
    delete ind;
    return true; 
}

bool HCoarseFineHistory::load(CoarseFineHistoryDesc *desc)
{
    int *attrs = (int *)desc;
    readIntAttr(".attr", attrs);
    return true;
}

bool HCoarseFineHistory::load(CoarseFineHistory *his, int count)
{
    typedef HOocArray<hdata::TInt, 32, 32> IData;
    bool stat;
    IData *ind = openDataStorage<IData>(".his", stat);
    if(!stat) {
        std::cout << "\n ERROR cannot open ooc storage .his";
        return false;
    }
    
    int nc = Round32(count)>>5;
    
    ind->readColumns((char *)his->value(), 0, nc);
    delete ind;
    return true;
}

}
