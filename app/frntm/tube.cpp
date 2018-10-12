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
    
    const int nu = 71;
    const float da = 6.28318531f / (float)nu;
    
    for(int i=0;i<nu;++i) {
        float phi = da * i;
        float wr = ScaledSin(phi * 8.8f + ur.randf1() * .8f, .3f, .65f)
                    + ScaledSin(phi * 4.1 + ur.randf1(), .0f, .4f)
                    + ScaledSin(phi * 1.7 + ur.randf1(), .0f, .2f);

        float radius = ellip.polarForm(phi) +  2.f * wr;
        addVertex(Vector3F(radius * cos(phi), 0.f, -radius * sin(phi) ));
    }

    FrontLine l[2];
    const Vector3F up(0.f, 2.3f, 0.f);
    const Quaternion lq(.169f, Vector3F::ZAxis);
    const Quaternion tq(.019f, Vector3F::YAxis);
    constexpr float cshrinking = .08f;
    
    for(int i=0;i<nu;++i) {
        l[0].addVertex(vertexPositionR(i), i);
    }
    
    l[0].closeLine();
    l[0].smooth(.14f);
    l[0].setMinEdgeLength(.43f);
    l[0].setWorldSpace(Matrix33F::IdentityMatrix);
    l[0].rotateLocalBy(lq);
    l[0].rotateLocalBy(tq);
    
    l[0].setShrinking(cshrinking);
    l[1].setShrinking(cshrinking);
    
    l[0].setDirection(up);
    l[1].setDirection(up);
    l[1].rotateLocalBy(lq);
    l[1].rotateLocalBy(tq);

    msher.attachMesh(this);

    for(int i=0;i<9;++i) {
        msher.setFrontId(i);
        FrontLine& la = l[i & 1];
        FrontLine& lb = l[(i+1) & 1];
        msher.advanceFront(lb, la);

        //std::cout<<" front a "<<la;

        lb.smooth();

        //lb.preAdvance();

        //std::cout<<" front b "<<lb;
        
        la.clearLine();
    }

    calculateVertexNormals();
}