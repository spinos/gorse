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

    EllipseFunc ellip;
    ellip.setA(10.f);
    ellip.setE(.34f);
    
    const int nu = 60;
    const float da = 6.28318531f / (float)nu;

    for(int i=0;i<nu;++i) {
        float phi = da * i;
        float wr = ScaledSin(phi * 9.8f + ur.randf1() * .8f, .3f, .6f)
                    + ScaledSin(phi * 4.1 + ur.randf1(), .0f, .3f)
                    + ScaledSin(phi * 1.7 + ur.randf1(), .0f, .2f);

        float radius = ellip.polarForm(phi) +  2.f * wr;
        addVertex(Vector3F(radius * cos(phi), 0.f, -radius * sin(phi) ));
    }

    FrontLine originLine;
    
    const Vector3F up(0.f, 1.9f, 0.f);
    const Quaternion lq(.139f, Vector3F::XAxis);
    const Quaternion tq(.059f, Vector3F::YAxis);
    constexpr float cshrinking = .05f;
    
    for(int i=0;i<nu;++i) {
        originLine.addVertex(vertexPositionR(i), i);
    }
    originLine.closeLine();

    originLine.smooth(.14f);

    originLine.setWorldSpace(Matrix33F::IdentityMatrix);
    originLine.rotateLocalBy(lq);
    originLine.rotateLocalBy(tq);
    originLine.setShrinking(cshrinking);
    originLine.setDirection(up);
    originLine.setMinEdgeLength(.4f);
    
    FrontLine *la = &originLine;
    FrontLine l[12];

    FrontMesher msher;
    msher.attachMesh(this);

    for(int i=0;i<12;++i) {
        msher.setFrontId(i+1);

        l[i].rotateLocalBy(lq);
        l[i].rotateLocalBy(tq);
        l[i].setShrinking(cshrinking);
        l[i].setDirection(up);
        l[i].setMinEdgeLength(.4f);

        msher.advanceFront(l[i], *la);

        std::cout<<"\n front "<<*la;

        l[i].smooth(.02f);

        la = &l[i];
    }

    calculateVertexNormals();
}