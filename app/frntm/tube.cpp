#include "tube.h"
#include <mesh/FrontMesher.h>
#include <math/EllipseFunc.h>
#include <math/wavefuncs.h>
#include <rng/Uniform.h>
#include <rng/Lehmer.h>

using namespace alo;

Tube::Tube()
{}

void Tube::createTube()
{
    Uniform<Lehmer> ur(7999999);

    FrontMesher msher;
    initAdaptableMesh();
    
    EllipseFunc ellip;
    ellip.setA(10.f);
    ellip.setE(.3f);
    
    const int nu = 64;
    const float da = 6.28318531f / (float)nu;
    
    for(int i=0;i<nu;++i) {
        float phi = da * i;
        float wr = ScaledSin(phi * 8.f + ur.randf1(), .1f, .6f)
                    + ScaledSin(phi * 4.3 + ur.randf1(), .0f, .2f)
                    + ScaledSin(phi * 1.7 + ur.randf1(), .0f, .1f);

        float radius = ellip.polarForm(phi) +  2.f * wr;
        addVertex(Vector3F(radius * cos(phi), 0.f, -radius * sin(phi) ));
    }

    FrontLine l[2];
    Vector3F up(.1f, 3.f, 0.f);
    
    for(int i=0;i<nu;++i) {
        l[0].addVertex(vertexPositionR(i), i);
    }
    
    l[0].closeLine();
    //std::cout<<" front a "<<l[0];
    
    l[0].setShrinking(.16f);
    l[1].setShrinking(.17f);
    
    l[0].setDirection(up);
    l[1].setDirection(up);
    
    msher.attachMesh(this);

    for(int i=0;i<5;++i) {
        msher.setFrontId(i);
        FrontLine& la = l[i & 1];
        FrontLine& lb = l[(i+1) & 1];
        msher.advanceFront(lb, la);
        //std::cout<<" front b "<<lb;
        la.clearLine();
    }
    
    calculateVertexNormals();
}