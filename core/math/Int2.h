#ifndef ALO_INT_2_H
#define ALO_INT_2_H

namespace alo {
    
struct Int2 {
	
	Int2(): x(0), y(0)
	{}
	
	Int2(int a, int b)
	: x(a), y(b)
	{}
	
	void set(int a, int b) 
	{ x = a; y = b; }
	
	int x, y;
};

}

#endif

