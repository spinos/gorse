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

    DrawableObject *d = drawable();
    const int n = d->drawArrayLength();
    for(int i=0;i<n;++i) {
        Vector3F &pos = posnml[i * 2];
        pos.x += a - 10.f;
        pos.y += b - 10.f;
    }
    DrawableScene *scene = drawableScene();
    scene->enqueueEditDrawable(d);
}

void RootOps::addDrawableTo(DrawableScene *scene)
{ 
    YCylinder c;
    c.createCylinder(16, 8, 7.f, 31.f);

    c.createPositionNormalArray(posnml);
    c.createBarycentricCoordinates(baryc);

    DrawableObject *cly = scene->createDrawable();
    cly->setPosnml((const float *)posnml.c_data(), posnml.capacityByteSize());
    cly->setBarycentric((const float *)baryc.c_data(), baryc.capacityByteSize());
    cly->setDrawArrayLength(c.numIndices());
    setDrawable(cly);
    setDrawableScene(scene);
}

}

