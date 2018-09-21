/*
 *  GMaterial.h
 *  aloe
 *
 *  Created by jian zhang on 9/18/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "GMaterial.h"
#include "gl_heads.h"
#include <iostream>

namespace alo {

GMaterial::GMaterial() 
{
	m_Ambient[0] = 0.1;
    m_Ambient[1] =  0.1;
    m_Ambient[2] = 0.1;
    m_Ambient[3] = 1.0;
	m_Diffuse[0] = 0.8;
    m_Diffuse[1] = 0.8;
    m_Diffuse[2] = 0.8; 
    m_Diffuse[3] = 1.0;
	m_backDiffuse[0] = 1.0;
    m_backDiffuse[1] = 0.0;
    m_backDiffuse[2] = 0.0;
    m_backDiffuse[3] = 1.0;
	m_Specular[0] = 0.0;
    m_Specular[1] = 0.0;
    m_Specular[2] = 0.0;
    m_Specular[3] = 1.0;
	m_Emission[0] = 0.0;
    m_Emission[1] = 0.0;
    m_Emission[2] = 0.0;
    m_Emission[3] = 1.0;
	m_Shininess = 10.f;
}
	
GMaterial::GMaterial(const float* ambient,
		 const float* frontDiffuse,
            const float* backDiffuse,
		 const float* specular,
		 const float* emission,
		 float shininess)
{
	memcpy(m_Ambient, ambient, 16);
	memcpy(m_Diffuse, frontDiffuse, 16);
	memcpy(m_backDiffuse, backDiffuse, 16);
	memcpy(m_Specular, specular, 16);
	memcpy(m_Emission, emission, 16);
	m_Shininess = shininess;
}

void GMaterial::apply() const
{
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, m_Ambient );
	glMaterialfv( GL_FRONT, GL_DIFFUSE, m_Diffuse );
	glMaterialfv( GL_BACK, GL_DIFFUSE, m_backDiffuse );
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, m_Specular );
	glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, m_Emission );
	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, m_Shininess );
}

void GMaterial::applyColor() const
{ glMaterialfv( GL_FRONT, GL_DIFFUSE, m_Diffuse ); }

}