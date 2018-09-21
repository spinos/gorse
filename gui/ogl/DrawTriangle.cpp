/*
 *  DrawTriangle.cpp
 *  
 *
 *  Created by jian zhang on 1/14/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#include "DrawTriangle.h"
#include <math/Vector3F.h>
#include "gl_heads.h"

namespace alo {

DrawTriangle::DrawTriangle() :
m_triBufLength(0)
{}
	
DrawTriangle::~DrawTriangle()
{}

const int & DrawTriangle::triBufLength() const
{ return m_triBufLength; }

Vector3F * DrawTriangle::triNormalR()
{ return m_triNormalBuf.get(); }

Vector3F * DrawTriangle::triColorR()
{ return m_triColorBuf.get(); }
	
Vector3F * DrawTriangle::triPositionR()
{ return m_triPositionBuf.get(); }

const float * DrawTriangle::triNormalBuf() const
{ return (const float *)m_triNormalBuf.get(); }

const float * DrawTriangle::triPositionBuf() const
{ return (const float *)m_triPositionBuf.get(); }

const float * DrawTriangle::triColorBuf() const
{ return (const float *)m_triColorBuf.get(); }

void DrawTriangle::setTriDrawBufLen(const int & x)
{ 
	m_triBufLength = x;
	m_triNormalBuf.reset(new Vector3F[x]);
	m_triColorBuf.reset(new Vector3F[x]);
	m_triPositionBuf.reset(new Vector3F[x]);
}

void DrawTriangle::buildTriangleDrawBuf(const int & nt, const int * tri,
				const int & nv, const Vector3F * vertP, 
				const Vector3F * vertN,
				const Vector3F * vertC )
{
	setTriDrawBufLen(nt * 3);
	
	int i=0, j;
	for(;i<m_triBufLength;++i) {
		j = tri[i]; 
		m_triNormalBuf[i] = vertN[j];
		m_triPositionBuf[i] = vertP[j];
		m_triColorBuf[i] = vertC[j];
	}
	
}

void DrawTriangle::drawWiredTriangles() const
{
	glVertexPointer(3, GL_FLOAT, 0, (GLfloat*)m_triPositionBuf.get() );
	glDrawArrays(GL_TRIANGLES, 0, m_triBufLength);
}

void DrawTriangle::drawSolidTriangles() const
{
	glNormalPointer(GL_FLOAT, 0, (const GLfloat*)m_triNormalBuf.get() );
	glVertexPointer(3, GL_FLOAT, 0, (const GLfloat*)m_triPositionBuf.get() );
	glDrawArrays(GL_TRIANGLES, 0, m_triBufLength);
}

}
