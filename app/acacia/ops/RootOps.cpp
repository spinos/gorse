#include "RootOps.h"

namespace alo {
   
RootOps::RootOps()
{
	std::cout<<" root ops init";
}
    
void RootOps::update()
{
    QAttrib * aa = findAttrib("a");
    FloatAttrib *fa = static_cast<FloatAttrib *>(aa);
    QAttrib * ab = findAttrib("b");
    FloatAttrib *fb = static_cast<FloatAttrib *>(ab);
    float a;
    fa->getValue(a);
    float b;
    fb->getValue(b);
    std::cout<<" root ops update "<<a<<" "<<b;
}

}

