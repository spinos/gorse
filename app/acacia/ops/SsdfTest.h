/*
 *  SsdfTest.h
 *  aloe
 *
 *  Created by zhang on 18-2-21.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef SSDF_TEST_H
#define SSDF_TEST_H

namespace alo {

class Vector3F;

class BoundingBox;

namespace sds {

struct FZOrderCurve;

template<typename T>
class SpaceFillingVector;

}

namespace sdf {

template<typename Tc>
class SsdfBuildRule;

template<typename T, typename Tv, int P, int Q, typename Tr>
class SsdfBuilder;

class SsdField;

}

class BaseDistanceField;

}

struct PosSample;

class SsdfTest {

typedef alo::sds::FZOrderCurve SfcTyp;
	SfcTyp* m_sfc;

typedef alo::sdf::SsdfBuildRule<alo::sds::FZOrderCurve> BuildRuleTyp;
	BuildRuleTyp* m_buildRule;
	
typedef alo::sdf::SsdfBuilder<PosSample, alo::Vector3F, 4, 7, BuildRuleTyp > BuilderTyp;
	BuilderTyp* m_builder;
	
typedef alo::sdf::SsdField FieldTyp;
	FieldTyp* m_field;

public:

    SsdfTest();
	
	void buildSsdf(alo::sds::SpaceFillingVector<PosSample >* samples,
                const alo::BoundingBox& b);
           
	void drawGraph();
	void drawSamples();
    
protected:
	
private:

	void saveToFile(const char* filename);
	void drawGraph(const alo::BaseDistanceField& fld, float zmin, float zmax, 
					bool drawEdge, bool drawDist, bool drawNormal);
	
};

#endif
