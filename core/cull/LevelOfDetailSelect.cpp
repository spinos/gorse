/*
 *  LevelOfDetailSelect.cpp
 *  aloe
 *
 */

#include "LevelOfDetailSelect.h"
#include <math/Hexahedron.h>
#include <math/PerspectiveCamera.h>

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

void LevelOfDetailSelect::select(const Hexahedron &hexa, const PerspectiveCamera &camera)
{
	float r = hexa.size() * .5f;
	float d = camera.eyePosition().distanceTo(hexa.center()) - r;
	if(d < 0.f) {
		select(16777216);
		return;
	}
	float lod = r / (d * camera.tanhfov() );
	if(lod > 1.f) lod = 1.f;
	if(lod < 0.f) lod = 0.f;
	int s = camera.portWidth() * lod;
	s = s * s * .033f;
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

std::ostream& operator<<(std::ostream &output, const LevelOfDetailSelect & p) 
{
    output << p.m_val;
    if(p.m_state == LevelOfDetailSelect::stDecline) std::cout << " down ";
    else if(p.m_state == LevelOfDetailSelect::stIncrease) std::cout << " up ";
    else std::cout << " no change ";
    return output;
}

}
