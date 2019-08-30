/* 
 *  PlantMesher.cpp
 *  gorse
 *
 *  2019/8/17
 */
 
#include "PlantMesher.h"
#include "Plant.h"
#include "Stem.h"
#include "PlantProfile.h"
#include "StemProfile.h"
#include <geom/AdaptableMesh.h>
#include <tbl/TubularCrossSection.h>
#include <tbl/TubularProfile.h>
#include <tbl/TubularMesher.h>

namespace alo {

namespace morph {

PlantMesher::PlantMesher()
{}

PlantMesher::~PlantMesher()
{}

void PlantMesher::attach(AdaptableMesh *mesh)
{ m_mesh = mesh; }

void PlantMesher::triangulate(const Plant &pl, PlantProfile &prof)
{
    TubularMesher tumshr;
    tumshr.attach(m_mesh);
    
    const int n = pl.numStems();
    for(int i=0;i<n;++i) {
        const Stem *si = pl.stem(i);
        
        const TubularProfile *tf = si->profile();
        
        TubularCrossSection tucrs;
        si->getCrossSection(&tucrs);
    
        tumshr.triangulate(tucrs, *tf);
    }
}

void PlantMesher::triangulate(const Plant &pl, const float &alpha, 
                                PlantProfile &prof, StemProfile &stf)
{
    if(alpha > .99f) return triangulate(pl, prof);
    
    TubularMesher tumshr;
    tumshr.attach(m_mesh);
    
    const float fage = (float)prof.age() * (alpha + .001f);
    const int ageLimit = fage;
    
    const int n = pl.numStems();
    for(int i=0;i<n;++i) {
        const Stem *si = pl.stem(i);
        
        if(si->ageOffset() > ageLimit) {
/// no more stems are old enough
            break;
        }
        
        float fseg = si->getSegment(fage, stf);

        TubularMesher::Parameter param;
        param._toSegment = fseg;
        
        const TubularProfile *tf = si->profile();
        
        TubularCrossSection tucrs;
        stf.setCurrentSegment(fseg);
        si->getCrossSection(&tucrs, &stf);
    
        tumshr.triangulate(tucrs, *tf, &param);
    }
}

void PlantMesher::detach()
{ m_mesh->calculateVertexNormals(); }

AdaptableMesh *PlantMesher::mesh()
{ return m_mesh; }

}

}
