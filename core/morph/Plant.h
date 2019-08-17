/* 
 *  Plant.h
 *  gorse
 *
 *  physical structure of a plant
 *  2019/8/17
 */

#ifndef ALO_MORPH_PLANT_H
#define ALO_MORPH_PLANT_H

#include <math/Matrix33F.h>
#include <deque>

namespace alo {
    
class TubularCrossSection;

namespace morph {

class Stem;
class PlantProfile;
class StemProfile;

class Plant {

	std::deque<Stem *> m_stems;

public:

	Plant();
	virtual ~Plant();

	Stem *addStem(const Vector3F &pos, const Matrix33F &mat,
				const float &a0, const float &r0);
	void grow(PlantProfile &plp, StemProfile &stp);

	void end();

	int numStems() const;
	const Stem *stem(const int &i) const;
	void getTerminalBud(Vector3F &pos, Matrix33F &mat,
					StemProfile &stp, const int &i) const;
                    
    void getStemCrossSection(TubularCrossSection *tucrs,
                    const int &i, PlantProfile &plp) const;

protected:

private:

};

}

}

#endif