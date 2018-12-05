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

#include <iostream>

namespace alo {

class Hexahedron;
class PerspectiveCamera;
class LevelOfDetailSelect {

	enum StateTag {
		stUnknown = 0,
		stNoChange,
		stDecline,
		stIncrease
	};

	float m_val;
	unsigned char m_state;
	char m_padding[3];

public:

	LevelOfDetailSelect();

	void set(float x);

	const float &value() const;
	bool isStateChanged() const;

	void select(const Hexahedron &hexa, const PerspectiveCamera &camera);

	friend std::ostream& operator<<(std::ostream &output, const LevelOfDetailSelect & p);

private:
	void select(float x);

};
	
}

#endif
