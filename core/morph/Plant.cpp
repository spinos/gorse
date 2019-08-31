/* 
 *  Plant.cpp
 *  gorse
 *
 *  physical structure of a plant
 *  2019/8/17
 */

#include "Plant.h"
#include "Stem.h"
#include "PlantProfile.h"
#include "StemProfile.h"

namespace alo {

namespace morph {

Plant::Plant()
{}

Plant::~Plant()
{
	std::deque<Stem *>::iterator it = m_stems.begin();
	for(;it!=m_stems.end();++it) {
		delete *it;
	}
	m_stems.clear();
}

Stem *Plant::addStem(const Vector3F &pos, const Matrix33F &mat,
					const float &a0, const float &r0)
{
	Stem *st = new Stem;
	st->begin(pos, mat, a0, r0);
	m_stems.push_back(st);
	return st;
}

void Plant::grow(PlantProfile &plp, StemProfile &stp)
{
	const Vector3F gv = plp.getGrowVector();

	std::deque<Stem *>::iterator it = m_stems.begin();
	for(;it!=m_stems.end();++it) {
        Stem *cst = *it;
        if(cst->isMainAxis()) {
            const Float2 &ra = plp.mainAxisGrowSpeedRatio();
            cst->grow(gv * ra.x, plp.growSpeed().y * ra.y, stp);
        } else {
            float flow = 1.f;
            cst->getResourceFlow(flow, plp.resourceRatio());
            cst->grow(gv, plp.growSpeed().y * sqrt(flow), stp);
        }
	}
}

void Plant::end()
{
	std::deque<Stem *>::iterator it = m_stems.begin();
	for(;it!=m_stems.end();++it) {
		(*it)->end();
	}
}

int Plant::numStems() const
{ return m_stems.size(); }

Stem *Plant::stem(const int &i)
{ return m_stems[i]; }

const Stem *Plant::stem(const int &i) const
{ return m_stems[i]; }

void Plant::getTerminalBud(Vector3F &pos, Matrix33F &mat,
					StemProfile &stp, const int &i) const
{
	const Stem *st = stem(i);
	st->getTerminalBud(pos, mat, stp);
}

}

}