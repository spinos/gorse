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
    ellip.setE(.34f);
    
    const int nu = 72;
    const float da = 6.28318531f / (float)nu;
    
    for(int i=0;i<nu;++i) {
        float phi = da * i;
        float wr = ScaledSin(phi * 7.f + ur.randf1() * .9f, .13f, .65f)
                    + ScaledSin(phi * 4.1 + ur.randf1(), .0f, .4f)
                    + ScaledSin(phi * 1.7 + ur.randf1(), .0f, .2f);

        float radius = ellip.polarForm(phi) +  2.f * wr;
        addVertex(Vector3F(radius * cos(phi), 0.f, -radius * sin(phi) ));
    }

    FrontLine l[2];
    Vector3F up(.1f, 3.f, 0.f);
    
    for(int i=0;i<nu;++i) {
        l[0].addVertex(vertexPositionR(i), i);
    }
    
    l[0].closeLine();
    l[0].smooth(.1f);
    l[0].setMinEdgeLength(.43f);
    //std::cout<<" front a "<<l[0];
    
    constexpr float cshrinking = .23f;
    l[0].setShrinking(cshrinking);
    l[1].setShrinking(cshrinking);
    
    l[0].setDirection(up);
    l[1].setDirection(up);
    
    msher.attachMesh(this);

    for(int i=0;i<6;++i) {
        msher.setFrontId(i);
        FrontLine& la = l[i & 1];
        FrontLine& lb = l[(i+1) & 1];
        msher.advanceFront(lb, la);
        lb.smooth();
        //std::cout<<" front b "<<lb;
        la.clearLine();
    }
    
    calculateVertexNormals();
}