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
/// between stem and branch from axillary bud
	float m_axilAngle;
/// max bending per-node
    float m_bendingLimit;
    float m_currentSegment;

public:

	StemProfile();
	virtual ~StemProfile();

	void setSegmentsPerSeason(const int &x);
	void setNodeAngle(const float &x);
	void setAxilAngle(const float &x);
    void setBendingLimit(const float &x);
    void setCurrentSegment(const float &x);

	const int &segmentsPerSeason() const;
	const float &nodeAngle() const;
    float segmentAngle() const;
	const float &axilAngle() const;
    const float &bendingLimit() const;
    const float &currentSegment() const;

protected:

private:

};

}

}

#endif
