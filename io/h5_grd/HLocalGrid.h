/*
 *  HLocalGrid.h
 *
 *  2019/5/19
 */

#ifndef ALO_H_LOCAL_GRID_H
#define ALO_H_LOCAL_GRID_H

#include <h5/V1HBase.h>
#include <math/miscfuncs.h>
#include <h5/HOocArray.h>

namespace alo {

class HLocalGrid : public ver1::HBase {

public:

	HLocalGrid(const std::string &name);
	
	virtual bool verifyType();

	template<typename T>
	bool save(const T &grid);

	template<typename T>
	bool load(T &grid);
	
protected:

private:

};

template<typename T>
bool HLocalGrid::save(const T &grid)
{ 
	if(!hasNamedAttr(".is_local_grid") )
	    addIntAttr(".is_local_grid", 2);

	int d[2];
    d[0] = grid.resolution();
    d[1] = grid.numNonemptyCells();
	writeIntAttr(".is_local_grid", d );
    
    if(!hasNamedAttr(".n_ind") )
        addIntAttr(".n_ind", 1);
    int nind = grid.numIndices();
    writeIntAttr(".n_ind", &nind );
    
    if(!hasNamedAttr(".n_obj") )
        addIntAttr(".n_obj", 1);
    int nobj = grid.numObjects();
    writeIntAttr(".n_obj", &nobj );

    if(!hasNamedAttr(".fld_box") )
        addFloatAttr(".fld_box", 6);
    writeFloatAttr(".fld_box", (float *)&grid.fieldBox() );

    bool stat;

    typedef HOocArray<hdata::TInt, 32, 32> IntData;
    
    IntData *indd = createDataStorage<IntData>(".ind", true, stat);
    if(!stat) {
    	std::cout << "\n ERROR HLocalGrid cannot create ooc storage ind";
    	return false;
    }

    int nc = Round32(nind)>>5;
    
    indd->writeColumns((char *)grid.c_indices(), 0, nc);
    delete indd;
    
    SimpleBuffer<Int2> celm;
    grid.getCellMap(celm);
    
    IntData *celmd = createDataStorage<IntData>(".cell_map", true, stat);
    if(!stat) {
    	std::cout << "\n ERROR HLocalGrid cannot create ooc storage cell_map";
    	return false;
    }

    nc = Round32(celm.count() * 2)>>5;
    
    celmd->writeColumns((char *)celm.data(), 0, nc);
    delete celmd;
    
    IntData *celld = createDataStorage<IntData>(".cell_range", true, stat);
    if(!stat) {
    	std::cout << "\n ERROR HLocalGrid cannot create ooc storage cell_range";
    	return false;
    }

    nc = Round32(grid.numNonemptyCells() * 2)>>5;
    
    celld->writeColumns((char *)grid.c_values(), 0, nc);
    delete celld;

    std::cout<<"\n HLocalGrid save "<<pathToObject();
    grid.verbose();

	return true; 
}

template<typename T>
bool HLocalGrid::load(T &grid)
{ 
    int d[2];
    readIntAttr(".is_local_grid", d );
    grid.create(d[0]);
    
    Int2 *rangeVal = grid.createValues(d[1]);

    int nind;
    readIntAttr(".n_ind", &nind );
    int *ind = grid.createIndices(nind);
    
    int nobj;
    readIntAttr(".n_obj", &nobj );
    grid.setNumObjects(nobj);

    float b[6];
    readFloatAttr(".fld_box", b);
    grid.setAabb(b);

    bool stat;

    typedef HOocArray<hdata::TInt, 32, 32> IntData;
    
    IntData *indd = openDataStorage<IntData>(".ind", stat);
    if(!stat) {
        std::cout << "\n ERROR HIndexGrid cannot open ooc storage ind";
        return false;
    }

    int nc = Round32(nind)>>5;
    
    indd->readColumns((char *)ind, 0, nc);
    delete indd;
    
    IntData *celld = openDataStorage<IntData>(".cell_range", stat);
    if(!stat) {
        std::cout << "\n ERROR HIndexGrid cannot open ooc storage cell_range";
        return false;
    }

    nc = Round32(d[1] * 2)>>5;
    
    celld->readColumns((char *)rangeVal, 0, nc);
    delete celld;

    SimpleBuffer<Int2> celm;
    celm.resetBuffer(d[1]);
    
    IntData *celmd = openDataStorage<IntData>(".cell_map", stat);
    if(!stat) {
    	std::cout << "\n ERROR HLocalGrid cannot open ooc storage cell_map";
    	return false;
    }

    nc = Round32(d[1] * 2)>>5;
    
    celmd->readColumns((char *)celm.data(), 0, nc);
    delete celmd;
    
    grid.createCellMap(celm);

    std::cout<<"\n HLocalGrid load "<<pathToObject();
    grid.buildBvh();
    grid.verbose();

	return true; 
}

}

#endif
