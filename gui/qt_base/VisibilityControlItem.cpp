/*
 *  VisibilityControlItem.cpp
 *  aloe
 *
 */

#include "VisibilityControlItem.h"

namespace alo {

VisibilityControlItem::VisibilityControlItem(QGraphicsItem * parent) : StateControlItem(parent)
{ setState(Visible); }

VisibilityControlItem::~VisibilityControlItem()
{}

void VisibilityControlItem::InitializeStates()
{
	AddStatePixmap(UnknownVisibility, ":images/eyeHalf.png");
	AddStatePixmap(Hidden, ":images/eyeClose.png");
	AddStatePixmap(Visible, ":images/eyeOpen.png");
}

void VisibilityControlItem::setStateVisible(const bool &x)
{
	if(x) setState(Visible);
	else setState(Hidden);
}

void VisibilityControlItem::beginEditState()
{ 
	m_state0 = state();
	setState(UnknownVisibility); 
}
	
void VisibilityControlItem::endEditState()
{
	if(m_state0 == Visible) setState(Hidden);
	else setState(Visible);
}

bool VisibilityControlItem::isStateVisible() const
{ return state() == Visible; }

} /// end of alo
