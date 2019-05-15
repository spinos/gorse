/*
 *  ActivationControlItem.cpp
 *  aloe
 *
 */

#include "ActivationControlItem.h"

namespace alo {

ActivationControlItem::ActivationControlItem(QGraphicsItem * parent) : StateControlItem(parent)
{ setState(Disabled); }

ActivationControlItem::~ActivationControlItem()
{}

void ActivationControlItem::InitializeStates()
{
	AddStatePixmap(UnknownActivation, ":images/orangelight.png");
	AddStatePixmap(Disabled, ":images/redlight.png");
	AddStatePixmap(Enabled, ":images/greenlight.png");
}

void ActivationControlItem::beginEditState()
{ 
	m_state0 = state();
	setState(UnknownActivation); 
}
	
void ActivationControlItem::endEditState()
{
	if(m_state0 == Enabled) setState(Disabled);
	else setState(Enabled);
}

bool ActivationControlItem::isStateEnabled() const
{ return state() == Enabled; }

}
