/* 
 *  PlantProfile.h
 *  gorse
 *
 *  2019/8/17
 */

#ifndef ALO_MORPH_PLANT_PROFILE_H
#define ALO_MORPH_PLANT_PROFILE_H

#include <math/Matrix33F.h>
#include <math/Float2.h>

namespace alo {

namespace morph {

class PlantProfile {

/// position of root
	Vector3F m_pos;
/// initial and general direction of growth
	Vector3F m_v0;
	Vector3F m_vm;
/// num of seasons to grow
	int m_age;
/// (length, width) of growth per season
	Float2 m_growSpeed;
    float m_branchProbability;
/// change grow speed based on available resource
    float m_seasonalFactor;

public:

	PlantProfile();
	virtual ~PlantProfile();

	void setRootPosition(const Vector3F &q);
	void setGrowDirections(const Vector3F &v0, const Vector3F &vm);
	void setAge(const int &x);
	void setGrowSpeed(const Float2 &q);
    void setBranchProbability(const float &x);
    void setSeasonalFactor(const float &x);

	const Vector3F &rootPosition() const;
	const Vector3F &initialGrowDirection() const;
	const Vector3F &generalGrowDirection() const;
	const int &age() const;
	const Float2 &growSpeed() const;
    const float &branchProbability() const;
	Matrix33F getRootRotation() const;
	Vector3F getGrowVector() const;

protected:

private:

};

}

}

#endif
