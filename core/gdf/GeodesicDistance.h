/*
 *  GeodesicDistance.h
 *  gorse
 *
 *  2019/8/12
 */

#ifndef ALO_GDF_GEODESIC_DISTANCE_H
#define ALO_GDF_GEODESIC_DISTANCE_H

#include "HexahedronDistanceBuilder.h"
#include "HexahedronDistanceBuildRule.h"
#include <sds/SparseFieldGradientLookup.h>
#include <sds/SparseFieldGradientLookupRule.h>

namespace alo {
	
namespace gdf {

class GeodesicDistance {

public:

	GeodesicDistance() {}

	template<typename T, typename Ts>
	void computeHexahedronField(T &pnts, Ts *sfc,
						const BoundingBox &shapeBox);

};


template<typename T, typename Ts>
void GeodesicDistance::computeHexahedronField(T &pnts, Ts *sfc,
						const BoundingBox &shapeBox)
{
	typedef HexahedronDistanceBuildRule<Ts> FieldRuleTyp;
    FieldRuleTyp rule(sfc);
    rule.setP(6);
    rule.setBBox(shapeBox);
    
    typedef sds::SparseCubicField<float> FieldTyp;
    FieldTyp field;
    HexahedronDistanceBuilder<FieldTyp> builder;

    builder.attach<FieldRuleTyp>(&field, rule);
    builder.measure<T, FieldRuleTyp>(pnts, rule);    
    builder.detach<FieldRuleTyp>(rule);

    typedef sds::SparseFieldGradientLookupRule<Ts> LookupRuleTyp;
    LookupRuleTyp lookupRule(sfc);
    lookupRule.setP(6);
    lookupRule.setBBox(shapeBox);

    sds::SparseFieldGradientLookup<FieldTyp> reader;
    reader.attach(&field);

    Vector3F sum(0,0,0);
    int nupd = 0;

    const int ns = pnts.size();
    for(int i=0;i<ns;++i) {
        const float *p = (const float *)&pnts[i]._pos;
        reader.lookupGradient<LookupRuleTyp>(p, lookupRule);
        if(reader.isStatusUnknown()) continue;

        if(reader.isStatusUpdate()) nupd++;

        pnts[i]._geod = reader.interpolate(p);
        reader.interpolateGradient((float *)&pnts[i]._grad, p);

        sum += pnts[i]._grad;
    }

    sum *= 1.f / ns;

    std::cout << "\n geodesic distance field average grad " << sum;
    reader.detach();
}
	
}

}

#endif
