#include "FaceValue.h"

namespace alo {

FaceValue::FaceValue()
{ m_ind = -1; }

FaceValue::FaceValue(int x)
{ m_ind = x; }
	
int &FaceValue::ind()
{ return m_ind; }

const int &FaceValue::ind() const
{ return m_ind; }

}
