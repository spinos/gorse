/*
 *  VertexValue.h
 *  aloe
 *
 */

#ifndef ALO_VERTEX_VALUE_H
#define ALO_VERTEX_VALUE_H

#include <iostream>

namespace alo {

class Vector3F;

class VertexValue
{
	float m_cost;
	bool m_locked;
	bool m_onborder;
	bool m_isRingConcave;
	char m_padding;

public:

	VertexValue();
	~VertexValue();

	const float &cost() const;
	float &cost();
	void lock();
	void unlock();
    void setCost(float x);
	void setOnBorder(bool x);
	void setRingConcave(bool x);
	const bool &isOnBorder() const;
	const bool &isLocked() const;
	const bool &isRingConcave() const;

	friend std::ostream& operator<<(std::ostream &output, const VertexValue & p);

private:
	
};

}

#endif
