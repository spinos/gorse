/*
 *  VisibilityState.h
 *  aloe
 *
 *  value and changed state
 *  dormant if not visible for a certain number of cycles
 */

#ifndef ALO_VISIBILITY_STATE_H
#define ALO_VISIBILITY_STATE_H

namespace alo {
	
class VisibilityState {

	unsigned char m_val;
	bool m_changed;

public:

	VisibilityState();

	void setVisible(bool x);
	
	bool isVisible() const;
	bool isHidden() const;
	bool isStateChanged() const;
	bool isDormant() const;

private:
	
	enum VisibleTag {
		vtUnknown = 0,
		vtSleep = 244,
		vtDormant = 245,
		vtHidden = 254,
		vtNormal = 255 
	};

};

}

#endif
