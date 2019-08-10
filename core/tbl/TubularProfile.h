/*
 *  TubularProfile.h
 *  gorse
 *
 *  moving frame along segmented line
 *  each segment rotate to align with the next one
 *
 *  2019/8/10
 */

#ifndef ALO_TUBULAR_PROFILE_H
#define ALO_TUBULAR_PROFILE_H

#include <math/Matrix33F.h>
#include <math/Float2.h>
#include <deque>

namespace alo {

class TubularProfile {

/// per-segment displacement
	std::deque<Vector3F> m_disp;
/// per-segment (pitch, yaw)
	std::deque<Float2> m_rot;
/// initial position
	Vector3F m_p0;
/// initial rotation frame
/// x facing direction of growth
/// rotate y (pitch) then z (yaw) bending to next segment
	Matrix33F m_frm0;

public:

	TubularProfile();
	virtual ~TubularProfile();

	void begin(const Vector3F &p);
	void end();

	void operator<<(const Vector3F &p);

	int numSegments() const;
/// iseg-th segment initial postion and rotation, position and rotation change
	void getSegment(Vector3F &pos, Matrix33F &frm, 
			Vector3F &disp, Float2 &pitchYaw,
			const int iseg) const;

protected:

private:

};
	
}

#endif
