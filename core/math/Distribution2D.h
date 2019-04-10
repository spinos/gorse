/*
 *  Distribution2D.h
 *  aloe
 *
 *  discrete distribution in (u,v)
 *
 *  Created by jian zhang on 3/30/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_DISTRIBUTION_2_D_H
#define ALO_DISTRIBUTION_2_D_H

#include "Array2.h"
#include "VectorN.h"

namespace alo {
	
class Distribution2D {
	
/// probability density function along v for each column
	Array2<float> m_vpdf;
/// cumulative distribution function along v for each column
	Array2<float> m_vcdf;
/// pdf and cdf along u
	VectorN<float> m_updf;
	VectorN<float> m_ucdf;
/// (nu, nv, nu * nv)
	int m_dim[3];
	float m_invDim[2];
	
public:

	Distribution2D();
	
	void create(int nu, int nv);
	
	void compute();
/// r random number [0,1] in 2D
/// sample via cdf in u, then in v
/// uv [0,1] in 2D
/// pdf(u,v) <- updf * vpdf * nu & nv
	void sample(float* uv, float& pdf, const float* r) const;
/// iu [0, nu]
/// iv [0, nv]
	void lookup(float& pdf, const int& iu, const int& iv) const;
/// i-th column in vpdf
	float* vpdf(int i);
	const float* c_vpdf(int i) const;
	
	const int& uDim() const;
	const int& vDim() const;
	
private:
	
	void computeVPdf(int i);
	void computeVCdf(int i);
	void computeUPdf();
	void computeUCdf();
	
};

}

#endif

