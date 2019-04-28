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

}

#endif
