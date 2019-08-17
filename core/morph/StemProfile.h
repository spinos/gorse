/* 
 *  StemProfile.h
 *  gorse
 *
 *  2019/8/17
 */

#ifndef ALO_MORPH_STEM_PROFILE_H
#define ALO_MORPH_STEM_PROFILE_H

namespace alo {

namespace morph {

class StemProfile {

/// num of segments per season
	int m_segmentsPerSeason;
/// rotation between nodes
	float m_nodeAngle;
/// between parent stem and branch
	float m_lateralAngle;
/// max bending per-node
    float m_bendingLimit;

public:

	StemProfile();
	virtual ~StemProfile();

	void setSegmentsPerSeason(const int &x);
	void setNodeAngle(const float &x);
	void setLateralAngle(const float &x);
    void setBendingLimit(const float &x);

	const int &segmentsPerSeason() const;
	const float &nodeAngle() const;
	const float &lateralAngle() const;
    const float &bendingLimit() const;

protected:

private:

};

}

}

#endif
