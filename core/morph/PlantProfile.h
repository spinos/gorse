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
#include <math/Int2.h>

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
/// must stop at age
/// x is main, y is secondary
    Int2 m_stopAge;
/// (length, width) of growth per season
	Float2 m_growSpeed;
    float m_branchProbability;
/// change grow speed based on available resource
    float m_seasonalFactor;
/// first season to branch
    int m_minBranchSeason;
/// terminal bud is lost, stop growing longer
/// continue to grow thicker
/// has no effect on main axis
    float m_stopProbability;
/// how much resources are going to grow child branches
/// relative to what is going to grow the parent
    float m_resourceRatio;

public:

	PlantProfile();
	virtual ~PlantProfile();

	void setRootPosition(const Vector3F &q);
	void setGrowDirections(const Vector3F &v0, const Vector3F &vm);
	void setAge(const int &x);
    void setStopAge(const Int2 &x);
	void setGrowSpeed(const Float2 &q);
    void setBranchProbability(const float &x);
    void setSeasonalFactor(const float &x);
    void setMinBranchSeason(const int &x);
    void setStopProbability(const float &x);
    void setResourceRatio(const float &x);

	const Vector3F &rootPosition() const;
	const Vector3F &initialGrowDirection() const;
	const Vector3F &generalGrowDirection() const;
	const int &age() const;
    const Int2 &stopAge() const;
	const Float2 &growSpeed() const;
    const float &branchProbability() const;
    const int &minBranchSeason() const;
    const float &stopProbability() const;
    const float &resourceRatio() const;
	Matrix33F getRootRotation() const;
	Vector3F getGrowVector() const;

protected:

private:

};

}

}

#endif
