#include "tube.h"
#include <mesh/FrontMesher.h>

using namespace alo;

Tube::Tube()
{}

void Tube::createTube()
{
    FrontMesher msher;
    initAdaptableMesh();
    
    const float radius = 9.f;
    const int nu = 12;
    const float da = 6.28318531f / (float)nu;
    
    for(int i=0;i<nu;++i) {
        addVertex(Vector3F(radius * cos(da * i), 0.f, -radius * sin(da * i) ));
    }

    FrontLine l[2];
    Vector3F up(1.f, 7.f, 0.f);
    
    for(int i=0;i<nu;++i)
        l[0].addVertex(vertexPositionR(i), up, i);
    
    l[0].closeLine();
    std::cout<<" front a "<<l[0];
    
    msher.attachMesh(this);

    for(int i=0;i<5;++i) {
        msher.setFrontId(i);
        FrontLine& la = l[i & 1];
        FrontLine& lb = l[(i+1) & 1];
        msher.advanceFront(lb, la);
        std::cout<<" front b "<<lb;
        la.clearLine();
    }
    
    calculateVertexNormals();
}