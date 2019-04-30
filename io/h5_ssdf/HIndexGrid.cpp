/*
 *  HIndexGrid.cpp
 *
 *  2019/4/28
 */

#include "HIndexGrid.h"
#include <grd/IndexGrid.h>
#include <h5/HOocArray.h>

namespace alo {

HIndexGrid::HIndexGrid(const std::string &name) :
HBase(name)
{}

bool HIndexGrid::verifyType()
{
	return hasNamedAttr(".is_index_grid");
}

bool HIndexGrid::save(const grd::IndexGrid &grid)
{ 
	if(!hasNamedAttr(".is_index_grid") )
	    addIntAttr(".is_index_grid", 1);

	int d = grid.resolution();
	writeIntAttr(".is_index_grid", &d );
    
    if(!hasNamedAttr(".n_ind") )
        addIntAttr(".n_ind", 1);
    int nind = grid.numIndices();
    writeIntAttr(".n_ind", &nind );

    if(!hasNamedAttr(".aabb") )
        addFloatAttr(".aabb", 6);
    writeFloatAttr(".aabb", (float *)grid.aabb() );

	typedef HOocArray<hdata::TFloat, 32, 32> FltData;
	bool stat;
    FltData *distd = createDataStorage<FltData>(".grid_dist", true, stat);
    if(!stat) {
    	std::cout << "\n ERROR HIndexGrid cannot create ooc storage grid_dist";
    	return false;
    }
    
    int nc = Round32(grid.numValues())>>5;
    
    distd->writeColumns((char *)grid.c_value(), 0, nc);
    delete distd;

    typedef HOocArray<hdata::TInt, 32, 32> IntData;
    IntData *celld = createDataStorage<IntData>(".cell_range", true, stat);
    if(!stat) {
    	std::cout << "\n ERROR HIndexGrid cannot create ooc storage cell_range";
    	return false;
    }

    nc = Round32(grid.numCells() * 2)>>5;
    
    celld->writeColumns((char *)grid.c_cell(), 0, nc);
    delete celld;

    IntData *indd = createDataStorage<IntData>(".ind", true, stat);
    if(!stat) {
    	std::cout << "\n ERROR HIndexGrid cannot create ooc storage ind";
    	return false;
    }

    nc = Round32(grid.numIndices())>>5;
    
    indd->writeColumns((char *)grid.c_indices(), 0, nc);
    delete indd;

    std::cout<<"\n HIndexGrid save "<<pathToObject();
    grid.verbose();

	return true; 
}

bool HIndexGrid::load(grd::IndexGrid &grid)
{ 
    int d;
    readIntAttr(".is_index_grid", &d );
    grid.create(d);

    int nind;
    readIntAttr(".n_ind", &nind );
    int *ind = grid.createIndices(nind);

    float b[6];
    readFloatAttr(".aabb", b);
    grid.setAabb(b);

    typedef HOocArray<hdata::TFloat, 32, 32> FltData;
    bool stat;
    FltData *distd = openDataStorage<FltData>(".grid_dist", stat);
    if(!stat) {
        std::cout << "\n ERROR HIndexGrid cannot open ooc storage grid_dist";
        return false;
    }

    int nc = Round32(grid.numValues())>>5;
    
    distd->readColumns((char *)grid.value(), 0, nc);
    delete distd;

    typedef HOocArray<hdata::TInt, 32, 32> IntData;
    IntData *celld = openDataStorage<IntData>(".cell_range", stat);
    if(!stat) {
        std::cout << "\n ERROR HIndexGrid cannot open ooc storage cell_range";
        return false;
    }

    nc = Round32(grid.numCells() * 2)>>5;
    
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

    std::cout<<"\n HIndexGrid load "<<pathToObject();
    grid.verbose();

	return true; 
}

} /// end of namespace alo
