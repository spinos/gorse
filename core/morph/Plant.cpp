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
#include <tbl/TubularCrossSection.h>

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
		(*it)->grow(gv, plp.growSpeed().y, stp);
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

const Stem *Plant::stem(const int &i) const
{ return m_stems[i]; }

void Plant::getTerminalBud(Vector3F &pos, Matrix33F &mat,
					StemProfile &stp, const int &i) const
{
	const Stem *st = stem(i);
	st->getTerminalBud(pos, mat, stp);
}

void Plant::getStemCrossSection(TubularCrossSection *tucrs,
                    const int &i, PlantProfile &plp) const
{
    const Stem *si = stem(i);
    const float &r = si->radius0();
    tucrs->create(10, r, r);
}

}

}