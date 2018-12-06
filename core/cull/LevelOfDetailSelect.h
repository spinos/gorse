/*
 *  LevelOfDetailSelect.h
 *  aloe
 *
 *  compute lod value in vertex count
 *  screen-space size
 *  s = w r / (d tan(theta/2))
 *  d is distance from camera to object
 *  theta is horizontal field of view in radians
 *  r is radius of object
 *  w is image width
 *  desired vertex count is proportional to s * s
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

	int m_val;
	unsigned short m_minValue;
	unsigned char m_state;
	char m_padding;

public:

	LevelOfDetailSelect();

	void set(int x);
	void setMin(int x);

	const int &value() const;
	bool isStateChanged() const;

	void select(const Hexahedron &hexa, const PerspectiveCamera &camera);

	friend std::ostream& operator<<(std::ostream &output, const LevelOfDetailSelect & p);

private:
	void select(int x);

};
	
}

#endif
