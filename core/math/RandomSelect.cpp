/*
 *  RandomSelect.cpp
 *  gorse
 *
 *  2019/7/23
 */

#include "RandomSelect.h"

namespace alo {

RandomSelect::RandomSelect() :
m_count(0)
{}

RandomSelect::~RandomSelect()
{}

void RandomSelect::create(int x)
{ m_count = x; }

void RandomSelect::setValue(int x, int i)
{ m_val[i] = x; }

void RandomSelect::setWeight(float x, int i)
{ m_wei[i] = x; }

void RandomSelect::normalize()
{
	if(m_count < 2) return;
	float c = 0.f;
	for(int i=0;i<m_count;++i) {
		c += m_wei[i];
	}

	for(int i=0;i<m_count;++i) {
		m_wei[i] /= c;
	}

	for(int i=1;i<m_count;++i) {
		m_wei[i] += m_wei[i-1];
	}
}

const int &RandomSelect::count() const
{ return m_count; }

}
