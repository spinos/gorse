/* 
 *  Stem.h
 *  gorse
 *
 *  modular structure of a plant
 *  grows from a shoot with a single terminal (apical) bud
 *  for each season it grows a number of nodes
 *  each node has a number of lateral (axillary) buds
 *  from each bud can develop in a leaf and/or flower
 *  2019/8/17
 */

#ifndef ALO_MORPH_STEM_H
#define ALO_MORPH_STEM_H

#include <math/Matrix33F.h>
#include <vector>

namespace alo {

class TubularProfile;

namespace morph {

class StemProfile;

class Stem {

/// from which this stem is grown
    const Stem *m_parent;
/// one node per-segment
	TubularProfile *m_tube;
/// first node rotation around x
	float m_node0Ang;
/// number of seasons
	int m_age;
/// node as tube segment index
    std::vector<int> m_nodeIndex;
/// angle between tube segments
	std::vector<float> m_segmentAngles;
    
public:

	Stem();
	virtual ~Stem();
    
    void setParent(const Stem *x);

/// position, rotation, node0_angle, and radius
	void begin(const Vector3F &pos, const Matrix33F &mat,
				const float &a0, const float &radius0);

	void grow(const Vector3F &gv, const float &dWidth, StemProfile &stp);

	void end();

    const Stem *parent() const;
	const TubularProfile *profile() const;
	const int &age() const;
	const float &radius0() const;

	void getTerminalBud(Vector3F &pos, Matrix33F &mat, StemProfile &stp) const;
	void getTerminalBudRotation(Matrix33F &mat, StemProfile &stp) const;
	Vector3F getGrowDirection(const Vector3F &ref, StemProfile &stp) const;
/// lateral and terminal
    void getAllBuds(std::vector<Vector3F> &positions, std::vector<Matrix33F> &rotations) const;

protected:

	
private:

};

}

}

#endif
