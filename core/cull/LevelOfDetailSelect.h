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

	int m_val;
	int m_preVal;
	unsigned char m_state;
	char m_padding[3];

public:

	LevelOfDetailSelect();

	void set(int x);
    void revert();

	const int &value() const;
	bool isStateChanged() const;
	bool isIncreased() const;

	void select(const Hexahedron &hexa, const PerspectiveCamera &camera);
	void select(int x);

	friend std::ostream& operator<<(std::ostream &output, const LevelOfDetailSelect & p);

private:
	
};
	
}

#endif
