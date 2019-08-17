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

/// initial position and rotation
	void begin(const Vector3F &p, const Matrix33F &frm);
	void end();

	void operator<<(const Vector3F &p);
/// double n vertex
	void smooth();

    const Vector3F &pos0() const;
    const Matrix33F &frm0() const;
    const Vector3F &displacement(const int &i) const;
	int numSegments() const;
/// iseg-th segment initial postion and rotation, position and rotation change
	void getSegment(Vector3F &pos, Matrix33F &frm, 
			Vector3F &disp, Float2 &pitchYaw,
			const int iseg) const;
	Vector3F interpolatePosition(const float &alpha) const;
	Matrix33F interpolateRotation(const float &alpha) const;
/// num_radius <- num_displacement
/// each radius plus x
	void expandRadius(const float &x);

	template<typename Tr>
	void randomSegments(int n, const Float2 &pitchYaw,
						const float &mean, const float &size,
						Tr &rule);

	static Matrix33F calculateFrame0(const Vector3F &v0, const Vector3F &v1);

protected:

private:

};

template<typename Tr>
void TubularProfile::randomSegments(int n, const Float2 &pitchYaw,
						const float &mean, const float &size,
						Tr &rule)
{
	*this << rule.firstDirection();
	const float du = 1.f / (float)n;
	for(int i=1;i<n;++i) {
		*this << rule.genDirection(pitchYaw, mean, size, du * i);
	}
}
	
}

#endif
