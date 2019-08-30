/* 
 *  Monopodial.h
 *  gorse
 *
 *  in which the growth of the main axis continues throughout a plant’s life, as do the 
 *  second-order and third-order branches developing on the main axis. 
 *  The growth of the main axis may cease temporarily under unfavorable conditions, 
 *  for example, during the winter. Monopodial branching is characteristic of 
 *  many seed plants (spruce, oak, lily of the valley), green algae, fungi, 
 *  and leafy mosses.
 *  2019/8/17
 */

#ifndef ALO_MORPH_MONOPODIAL_H
#define ALO_MORPH_MONOPODIAL_H

#include "Plant.h"
#include "Stem.h"
#include "PlantProfile.h"
#include "StemProfile.h"

namespace alo {

namespace morph {

template<typename T>
class Monopodial {

	T *m_rng;

public:

	Monopodial(T *rng);
	virtual ~Monopodial();

	void grow(Plant *pl, PlantProfile &plp, StemProfile &stp);

protected:

private:
	void processBranching(Plant *pl, PlantProfile &plp, StemProfile &stp);
    void processStopping(Plant *pl, PlantProfile &plp, StemProfile &stp);
    
};

template<typename T>
Monopodial<T>::Monopodial(T *rng)
{ m_rng = rng; }

template<typename T>
Monopodial<T>::~Monopodial()
{}

template<typename T>
void Monopodial<T>::grow(Plant *pl, PlantProfile &plp, StemProfile &stp)
{
	const float rootAng = (m_rng->randf1() - .5f) * 6.28f;
	Stem *st = pl->addStem(plp.rootPosition(), plp.getRootRotation(),
								rootAng, .013f);

	for(int i=0;i<plp.age();++i) {
        processStopping(pl, plp, stp);
		if(i >= plp.minBranchSeason()) {
            plp.setCurrentSeason(i);
            processBranching(pl, plp, stp);
        }
		float sfac = 1.f + (m_rng->randf1() - .5f) * .3f;
		plp.setSeasonalFactor(sfac);
		pl->grow(plp, stp);
        
	}

	pl->end();
}

template<typename T>
void Monopodial<T>::processBranching(Plant *pl, PlantProfile &plp, StemProfile &stp)
{
	const int n = pl->numStems();
    if(n > 2000) {
        std::cout << "\n WARNING plant reached max n branch " << 2000;
        return;
    }
	Vector3F budPos;
	Matrix33F budMat;
	for(int i=0;i<n;++i) {
        Stem *pst = pl->stem(i);
        if(pst->isStopped()) continue;
        
        pst->setBranched(false);
		if(m_rng->randf1() > plp.branchProbability()) continue;
        
		pl->getTerminalBud(budPos, budMat, stp, i);
        const float axil = stp.axilAngle() * (1.f - m_rng->randf1() * .2f);
		Matrix33F::rotateAroundLocalY(budMat, axil);

		const float branchAng = -1.57f - stp.nodeAngle();
		Stem *cst = pl->addStem(budPos, budMat,
								branchAng, .015f);
        cst->setParent(pl->stem(i));
        cst->setAgeOffset(plp.currentSeason());
        
        pst->addChild(cst);
        pst->setBranched(true);
	}
}

template<typename T>
void Monopodial<T>::processStopping(Plant *pl, PlantProfile &plp, StemProfile &stp)
{
    const int n = pl->numStems();
	for(int i=0;i<n;++i) {
        Stem *cst = pl->stem(i);
        if(cst->numChildren() < 1) continue;
        
        bool toStop = false;
        if(cst->parent() == nullptr) {
            if(cst->age() >= plp.stopAge().x)
                toStop = true;
        } else {
            toStop = cst->age() >= plp.stopAge().y
                || m_rng->randf1() < plp.stopProbability();
        }
        
        if(toStop) cst->setStopped();
	}
}

}

}

#endif
