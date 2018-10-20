#include "RootOps.h"
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>
#include <geom/YCylinder.h>

namespace alo {
   
RootOps::RootOps()
{
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

void RootOps::addDrawableTo(DrawableScene *scene)
{ 
    YCylinder c;
    c.createCylinder(16, 8, 7.f, 31.f);

    c.createPositionArray(posarr);
    c.createNormalArray(nmlarr);
    c.createBarycentricCoordinates(baryc);

    DrawableObject *cly = scene->createDrawable();
    cly->setData((const float *)posarr.c_data(),
                (const float *)nmlarr.c_data(),
                (const float *)baryc.c_data(),
                c.numIndices());
}

}

