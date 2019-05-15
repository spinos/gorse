/*
 *  VisibilityControlItem.h
 *  aloe
 *
 */

#ifndef ALO_VISIBILITY_CONTROL_ITEM_H
#define ALO_VISIBILITY_CONTROL_ITEM_H

#include "StateControlItem.h"

namespace alo {

class VisibilityControlItem : public StateControlItem {

public:
    
	VisibilityControlItem(QGraphicsItem * parent = 0);
	virtual ~VisibilityControlItem();
    
	static void InitializeStates();

	void beginEditState();
	void endEditState();

	bool isStateVisible() const;

protected:

private:
	enum VisibilityState {
		UnknownVisibility = 255,
		Hidden,
		Visible
	};

	int m_state0;

};

}

#endif
