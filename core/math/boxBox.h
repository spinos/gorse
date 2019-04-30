#ifndef ALO_BOX_BOX_H
#define ALO_BOX_BOX_H

namespace alo {
	
inline bool boxIntersectBox(const float *a, const float *b)
{
	for(int i=0;i<3;++i) {
		if(a[i] > b[i+3]) return false;
		if(a[i+3] < b[i]) return false;
	}
	return true;
}

/// expand b by a
inline void expandAabbByAabb(float *b, const float *a)
{
	if(b[0] > a[0]) b[0] = a[0];
	if(b[1] > a[1]) b[1] = a[1];
	if(b[2] > a[2]) b[2] = a[2];
	if(b[3] < a[3]) b[3] = a[3];
	if(b[4] < a[4]) b[4] = a[4];
	if(b[5] < a[5]) b[5] = a[5];
}

}

#endif
