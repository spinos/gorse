/*
 *  LevelOfDetailSelect.cpp
 *  aloe
 *
 */

#include "LevelOfDetailSelect.h"
#include <math/Hexahedron.h>

namespace alo {

LevelOfDetailSelect::LevelOfDetailSelect() :
m_val(0),
m_preVal(0),
m_state(stUnknown)
{}

const int &LevelOfDetailSelect::value() const
{ return m_val; }

bool LevelOfDetailSelect::isStateChanged() const
{ return m_state > stNoChange; }

void LevelOfDetailSelect::set(int x)
{ m_val = x; }

void LevelOfDetailSelect::select(const Hexahedron &hexa, const LevelOfDetailParam &param)
{
	float r = hexa.size();
	float d = param._viewPoint.distanceTo(hexa.center());
	if(d < r) {
		select(16777217 - d);
		return;
	}
	int s = param._screenWidth * r / ((d - r) * param._tanhfov );
	s *= logf(.23f * (s * s) + 1.f);
	select(s);
}

void LevelOfDetailSelect::select(int x)
{
	if(x < m_val) {
		m_state = stDecline;
		m_preVal = m_val;
		m_val = x;
		return;
	} 

	if(x > m_val) {
		m_state = stIncrease;
		m_preVal = m_val;
		m_val = x;
		return;
	} 

	m_state = stNoChange;
}

void LevelOfDetailSelect::revert()
{ 
	m_val = m_preVal; 
	m_state = stNoChange;
}

bool LevelOfDetailSelect::isIncreased() const
{ return m_state == stIncrease; }

std::ostream& operator<<(std::ostream &output, const LevelOfDetailSelect & p) 
{
    output << p.m_val;
    if(p.m_state == LevelOfDetailSelect::stDecline) std::cout << " down ";
    else if(p.m_state == LevelOfDetailSelect::stIncrease) std::cout << " up ";
    else std::cout << " no change ";
    return output;
}

}
