#ifndef ALO_WAVE_FUNCS_H
#define ALO_WAVE_FUNCS_H

#include "miscfuncs.h"

namespace alo {
	
inline float ScaledSin(float theta, float a, float b) {
	float r = sin(theta);
	if(r > 0.f) r *= a;
	else r *= b;
	return r;
}

}

#endif
