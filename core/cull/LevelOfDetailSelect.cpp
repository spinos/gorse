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
m_val(0.f),
m_state(stUnknown)
{}

const float &LevelOfDetailSelect::value() const
{ return m_val; }

bool LevelOfDetailSelect::isStateChanged() const
{ return m_state > stNoChange; }

void LevelOfDetailSelect::set(float x)
{ m_val = x; }

void LevelOfDetailSelect::select(const Hexahedron &hexa, const PerspectiveCamera &camera)
{
	float r = hexa.size() * .5f;
	float d = camera.eyePosition().distanceTo(hexa.center()) - r * 2.1f;
	if(d < 0.f) {
		select(1.f);
		return;
	}
	float lod = r / (d * camera.tanhfov() );
	lod = .9f * lod + .1f *(lod * lod);
	if(lod > .99f) lod = 1.f;
	if(lod < .01f) lod = 0.f;
	select(lod);
}

void LevelOfDetailSelect::select(float x)
{
	if(x < m_val - .014f) {
		m_state = stDecline;
		m_val = x;
		return;
	} 

	if(x > m_val + .01f) {
		m_state = stIncrease;
		m_val = x;
		return;
	} 

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
