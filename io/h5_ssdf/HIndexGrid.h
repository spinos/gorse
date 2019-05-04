/*
 *  HIndexGrid.h
 *
 *  2019/5/4
 */

#ifndef ALO_H_INDEX_GRID_H
#define ALO_H_INDEX_GRID_H

#include <h5/V1HBase.h>

namespace alo {

namespace grd {
class IndexGrid;
}

class HIndexGrid : public ver1::HBase {

public:

	HIndexGrid(const std::string &name);
	
	virtual bool verifyType();

	bool save(const grd::IndexGrid &grid);
	bool load(grd::IndexGrid &grid);
	
protected:

private:

};

}

#endif
