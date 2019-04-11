/*
 *  Shader.cpp
 *  aloe
 *
 *  Created by jian zhang on 3/28/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#include "Shader.h"
#include <interface/RenderableScene.h>
#include <math/SpaceTransform.h>
#include <brdf/Lambert.h>
#include <brdf/CookTorrance.h>
#include <rng/Hammersley.h>
#include "light.h"
#include "EnvironmentLight.h"
#include <img/ExrImage.h>
#include "IntersectResult.h"

namespace alo {
	
Shader::Shader()
{
	m_tangentSpace = new SpaceTransform;
	m_lambert = new Lambert(m_tangentSpace);
	m_cook = new CookTorrance(m_tangentSpace);
}

void Shader::evaluate(float* col, Ray& aray, IntersectResult& isect,
				PrngTyp* rng, const RenderableScene* ascene)
{
	col[0] = col[1] = col[2] = 0.f;
	col[3] = 1.f;
	const EnvLightTyp* envl = ascene->environmentLight();
	
	Vector3F surfaceP = aray.travel(isect.rayDistance() - 1e-3f);
	
	m_tangentSpace->setRotation(isect.hitNormal() );
	m_cook->setWo(aray.direction() );
	
	Vector3F wi, contrib(0.f);
/// pdfA BRDF
/// pdfB light source
	float fr, pdfA, pdfB, cosTheta, cw, lod;
/// 50-50 chance
	if(rng->binary() ) {
/// sample brdf
		m_lambert->sample<Hammersley, PrngTyp >(wi, fr, cosTheta, pdfA, rng);

        envl->getPdf(pdfB, wi);

/// https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch20.html
/// strong brdf more blur
/// strong light source less blur
        lod = 1.f + log2f(pdfA / pdfB);
        envl->lookupRadiance(contrib, lod, isect.radianceState(), wi );
		
		cw = pdfA / (pdfA + pdfB);
		contrib.addTo(col, cw * fr * cosTheta / pdfA);
	} else {
/// sample light source	
		bool hasContrib = envl->sample<Hammersley, PrngTyp, Lambert >(wi, contrib, pdfB, isect.radianceState(), m_lambert, rng);
		if(!hasContrib) return;
		
		envl->getPdf(pdfB, wi);
		
		m_lambert->getFr(fr, pdfA, cosTheta, wi);
		
		lod = 1.f + log2f(pdfA / pdfB);
		envl->lookupRadiance(contrib, lod, isect.radianceState(), wi );
		
		cw = pdfB / (pdfA + pdfB);

		contrib.addTo(col, cw * fr * cosTheta / pdfB);
	}
	
		if(col[0] > 3.f) col[0] = 3.f;
		if(col[1] > 3.f) col[1] = 3.f;
		if(col[2] > 3.f) col[2] = 3.f;
}

}