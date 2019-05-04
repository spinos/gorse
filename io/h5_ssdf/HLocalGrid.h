/*
 *  HLocalGrid.h
 *
 *  2019/5/4
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
	    addIntAttr(".is_local_grid", 1);

	int d = grid.resolution();
	writeIntAttr(".is_local_grid", &d );
    
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
    IntData *celld = createDataStorage<IntData>(".cell_range", true, stat);
    if(!stat) {
    	std::cout << "\n ERROR HLocalGrid cannot create ooc storage cell_range";
    	return false;
    }

    int nc = Round32(grid.numCells() * 2)>>5;
    
    celld->writeColumns((char *)grid.c_cell(), 0, nc);
    delete celld;

    IntData *indd = createDataStorage<IntData>(".ind", true, stat);
    if(!stat) {
    	std::cout << "\n ERROR HLocalGrid cannot create ooc storage ind";
    	return false;
    }

    nc = Round32(grid.numIndices())>>5;
    
    indd->writeColumns((char *)grid.c_indices(), 0, nc);
    delete indd;

    std::cout<<"\n HLocalGrid save "<<pathToObject();
    grid.verbose();

	return true; 
}

template<typename T>
bool HLocalGrid::load(T &grid)
{ 
    int d;
    readIntAttr(".is_local_grid", &d );
    grid.create(d);

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
    IntData *celld = openDataStorage<IntData>(".cell_range", stat);
    if(!stat) {
        std::cout << "\n ERROR HIndexGrid cannot open ooc storage cell_range";
        return false;
    }

    int nc = Round32(grid.numCells() * 2)>>5;
    
    celld->readColumns((char *)grid.cell(), 0, nc);
    delete celld;

    IntData *indd = openDataStorage<IntData>(".ind", stat);
    if(!stat) {
        std::cout << "\n ERROR HIndexGrid cannot open ooc storage ind";
        return false;
    }

    nc = Round32(grid.numIndices())>>5;
    
    indd->readColumns((char *)ind, 0, nc);
    delete indd;

    std::cout<<"\n HLocalGrid load "<<pathToObject();
    grid.buildBvh();
    grid.verbose();

	return true; 
}

}

#endif
