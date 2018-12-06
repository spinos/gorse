/*
 *  VisibilityState.cpp
 *  aloe
 *
 */

#include "VisibilityState.h"

namespace alo {

VisibilityState::VisibilityState() :
m_val(vtNormal),
m_changed(false)
{}

void VisibilityState::setVisible(bool x)
{
	if(x) {
		m_changed = (m_val < vtNormal);
		m_val = vtNormal;
	} else {
		m_changed = (m_val > vtHidden);
		m_val--;
		if(m_val == vtUnknown) m_val = vtSleep;
	}
}
	
bool VisibilityState::isVisible() const
{ return m_val > vtHidden; }

bool VisibilityState::isHidden() const
{ return m_val == vtHidden; }

bool VisibilityState::isStateChanged() const
{ return m_changed; }

bool VisibilityState::isDormant() const
{ return m_val <= vtDormant; }

}
