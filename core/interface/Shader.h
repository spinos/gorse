/*
 *  Shader.h
 *  aloe
 *
 *
 *  Created by jian zhang on 3/28/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SHADER_H
#define ALO_SHADER_H

#include <math/Ray.h>
#include "prng.h"

namespace alo {

class IntersectResult; 
class RenderableScene;
class SpaceTransform;
class Lambert;
class CookTorrance;
	
class Shader {

	SpaceTransform* m_tangentSpace;
	Lambert* m_lambert;
	CookTorrance* m_cook;
	
public:

	Shader(); 
/// col output color
/// aray primary ray from eye to surface point
/// compute w Lr Fr cosTheta / pdf
/// w balanced heuristic weight of combined estimator pdf_a / (pdf_a + pdf_b)
/// Lr light source radiance
/// Fr BRDF
/// cosTheta wi.n
/// pdf probablity density function
	void evaluate(float* col, Ray& aray, IntersectResult& isect,
				PrngTyp* rng, const RenderableScene* ascene);
	
private:

};

}

#endif