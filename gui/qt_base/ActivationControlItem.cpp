/*
 *  ActivationControlItem.cpp
 *  aloe
 *
 *  2019/5/26
 */

#include "ActivationControlItem.h"

namespace alo {

ActivationControlItem::ActivationControlItem(QGraphicsItem * parent) : StateControlItem(parent)
{ setState(SwitchOff); }

ActivationControlItem::~ActivationControlItem()
{}

void ActivationControlItem::InitializeStates()
{
	AddStatePixmap(UnknownActivation, ":images/orangelight.png");
	AddStatePixmap(SwitchOff, ":images/redlight.png");
	AddStatePixmap(Activated, ":images/greenlight.png");
}

void ActivationControlItem::setStateActivated(const bool &x)
{
	if(x) setState(Activated);
	else setState(SwitchOff);
}

void ActivationControlItem::beginEditState()
{ 
	m_state0 = state();
	setState(UnknownActivation); 
}
	
void ActivationControlItem::endEditState()
{
	if(m_state0 == Activated) setState(SwitchOff);
	else setState(Activated);
}

bool ActivationControlItem::isStateEnabled() const
{ return state() == Activated; }

} /// end of alo
