/*
 *  SampleWidget.cpp
 *  envt
 *
 *  Created by jian zhang on 9/14/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */
#include <QtGui>
#include "SampleWidget.h"
#include <rng/Lehmer.h>
#include <rng/Uniform.h>
#include <math/linspace.h>
#include <math/Plot1D.h>
#include <math/miscfuncs.h>
#include <interface/EnvironmentLight.h>
#include <rng/Hammersley.h>
#include <math/SpaceTransform.h>
#include <brdf/Lambert.h>
#include <brdf/CookTorrance.h>
#include <ctime>

using namespace alo;

SampleWidget::SampleWidget(QWidget *parent) : Plot1DWidget(parent)
{
	setBound(0, TWO_PIF, 4, PIF, 0, 2);
	m_tangentSpace = new SpaceTransform;
	m_lambert = new Lambert(m_tangentSpace);
	m_cook = new CookTorrance(m_tangentSpace);
	
	m_tangentSpace->setRotation(Vector3F(0.f,1.f,0.f) );
	m_cook->setWo(Vector3F(-1,-1,0).normal() );
}

SampleWidget::~SampleWidget()
{}

void SampleWidget::generateSamples(const EnvLightTyp* lit)
{
	std::time_t secs = std::time(0);
	Uniform<Lehmer> lmlcg(secs);
	SampleState radianceState;
	radianceState.setAccess(SampleState::Wrap, SampleState::Clamp);
	radianceState.setFilter(SampleState::Nearest);
	radianceState.setChannel(SampleState::RGB);
	
	static const int ns = 10000;
	std::cout<<"\n draw "<<ns<<" samples from envrionment";
	
	alo::Plot1D * m_sphPlot = new Plot1D;
    m_sphPlot->create(ns);
	
	Vector3F wi;
	Vector3F lr;
	float pdf;
	float phiTheta[2];
	for(int i=0;i<ns;++i) {
		lit->sample<Hammersley, Uniform<Lehmer>, Lambert >(wi, lr, pdf, &radianceState, m_lambert, &lmlcg);
		
		lit->worldToLatlong(phiTheta, wi);
		phiTheta[0] *= TWO_PIF;
		phiTheta[1] *= PIF;
		
		m_sphPlot->x()[i] = phiTheta[0];
		m_sphPlot->y()[i] = phiTheta[1];
		
	}
	
	m_sphPlot->setColor(0,0.4,0.3);
	m_sphPlot->setGeomType(Plot1D::GtMark);
	
	addVectorPlot(m_sphPlot);
	
	std::cout.flush();
}
