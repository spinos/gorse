/*
 *  LevelOfDetailSelect.h
 *  aloe
 *
 *  compute lod value
 *  screen-space radius
 *  s = r / (d tan(theta/2))
 *  d is distance from camera to object
 *  theta is horizontal field of view in radians
 *  r is radius of object
 *
 */

#ifndef ALO_LEVEL_OF_DETAIL_SELECT_H
#define ALO_LEVEL_OF_DETAIL_SELECT_H

namespace alo {

class LevelOfDetailSelect {

public:
	LevelOfDetailSelect();
};
	
}

#endif
