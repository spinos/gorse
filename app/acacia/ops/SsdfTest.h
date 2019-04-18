/*
 *  SsdfTest.h
 *  acacia
 *
 *  Created by zhang on 19-4-18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ACAC_SSDF_TEST_H
#define ACAC_SSDF_TEST_H

#include "DrawableOps.h"

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

//class BaseDistanceField;

struct SurfaceSample;

class SsdfTest : public DrawableOps {

typedef sds::FZOrderCurve SfcTyp;
	SfcTyp* m_sfc;

typedef sdf::SsdfBuildRule<sds::FZOrderCurve> BuildRuleTyp;
	BuildRuleTyp* m_buildRule;
	
typedef sdf::SsdfBuilder<SurfaceSample, float, 5, 7, BuildRuleTyp > BuilderTyp;
	BuilderTyp* m_builder;
	
typedef sdf::SsdField FieldTyp;
	FieldTyp* m_field;

	static AFileDlgProfile SWriteProfile;

public:

	enum { Type = 704002 };

    SsdfTest();
    virtual ~SsdfTest();

    virtual void update() override;
    virtual void addDrawableTo(DrawableScene *scene) override;
	virtual bool hasMenu() const override;
    virtual void getMenuItems(std::vector<std::pair<std::string, int > > &ks) const override;
    virtual void recvAction(int x) override;
    virtual AFileDlgProfile *writeFileProfileR () const override;
	
protected:
	
private:
	void testIt();
	void buildSsdf(sds::SpaceFillingVector<SurfaceSample >* samples,
                const BoundingBox& b);
	void computeMesh();
	void saveToFile(const std::string &filename);
	//void drawGraph(const alo::BaseDistanceField& fld, float zmin, float zmax, 
	//				bool drawEdge, bool drawDist, bool drawNormal);
	
};

}

#endif
