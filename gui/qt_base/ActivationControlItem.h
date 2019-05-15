/*
 *  ActivationControlItem.h
 *  aloe
 *
 */

#ifndef ALO_ACTIVATION_CONTROL_ITEM_H
#define ALO_ACTIVATION_CONTROL_ITEM_H

#include "StateControlItem.h"

namespace alo {

class ActivationControlItem : public StateControlItem {

public:
	ActivationControlItem(QGraphicsItem * parent = 0);
	virtual ~ActivationControlItem();

	static void InitializeStates();

	void beginEditState();
	void endEditState();

	bool isStateEnabled() const;

protected:

private:
	enum ActivationState {
		UnknownActivation = 258,
		Disabled,
		Enabled
	};

	int m_state0;

};

}

#endif
