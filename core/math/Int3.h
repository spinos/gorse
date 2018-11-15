#ifndef ALO_INT_3_H
#define ALO_INT_3_H

namespace alo {
    
struct Int3 {
	
	Int3(): x(0), y(0), z(0)
	{}
	
	Int3(int a, int b, int c)
	: x(a), y(b), z(c)
	{}
	
	void set(int a, int b, int c) 
	{ x = a; y = b; z = c; }
	
	int x, y, z;
};

}

#endif
