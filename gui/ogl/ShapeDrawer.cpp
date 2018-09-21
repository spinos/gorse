/*
 *  ShapeDrawer.cpp
 *  
 *
 *  Created by zhang on 18-2-16.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#include "ShapeDrawer.h"
#include "gl_heads.h"
#include <math/Matrix44F.h>

namespace alo {

const float ShapeDrawer::UnitColor[4][4] = {
{1.f, 0.f, 0.f, 1.f},
{0.f, 1.f, 0.f, 1.f},
{0.f, 0.f, 1.f, 1.f},
{0.f, 0.f, 0.f, 1.f}
};

const float ShapeDrawer::PyramidV[18][4] = {
{-1.f,-.309f, .309f, 1.f}, /// bottom
{-1.f,-.309f,-.309f, 1.f},
{ 0.f, 0.f, 0.f, 1.f},
{-1.f, .309f,-.309f, 1.f}, /// top
{-1.f, .309f, .309f, 1.f},
{ 0.f, 0.f, 0.f, 1.f},
{-1.f,-.309f,-.309f, 1.f}, /// back
{-1.f, .309f,-.309f, 1.f},
{ 0.f, 0.f, 0.f, 1.f},
{-1.f, .309f, .309f, 1.f}, /// front
{-1.f,-.309f, .309f, 1.f},
{ 0.f, 0.f, 0.f, 1.f},
{-1.f,-.309f,-.309f, 1.f}, /// left1
{-1.f,-.309f, .309f, 1.f},
{-1.f, .309f,-.309f, 1.f},
{-1.f, .309f,-.309f, 1.f}, /// left2
{-1.f,-.309f, .309f, 1.f},
{-1.f, .309f,.309f, 1.f},
};

const float ShapeDrawer::PyramidN[18][4] = {
{.9554271f,-.2952269f, 0.f, 1.f}, /// bottom
{.9554271f,-.2952269f, 0.f, 1.f},
{.9554271f,-.2952269f, 0.f, 1.f},
{.9554271f, .2952269f, 0.f, 1.f}, /// top
{.9554271f, .2952269f, 0.f, 1.f},
{.9554271f, .2952269f, 0.f, 1.f},
{.9554271f, 0.f,-.2952269f, 1.f}, /// back
{.9554271f, 0.f,-.2952269f, 1.f},
{.9554271f, 0.f,-.2952269f, 1.f},
{.9554271f, 0.f, .2952269f, 1.f}, /// front
{.9554271f, 0.f, .2952269f, 1.f},
{.9554271f, 0.f, .2952269f, 1.f},
{-1.f,0.f,0.f, 1.f}, /// left1
{-1.f,0.f,0.f, 1.f},
{-1.f,0.f,0.f, 1.f},
{-1.f,0.f,0.f, 1.f}, /// left2
{-1.f,0.f,0.f, 1.f},
{-1.f,0.f,0.f, 1.f},
};

const float ShapeDrawer::CenterBoxEdgeV[24][3] = {
{-1.f, -1.f, -1.f}, /// x
{ 1.f, -1.f, -1.f},
{-1.f,  1.f, -1.f},
{ 1.f,  1.f, -1.f},
{-1.f, -1.f,  1.f},
{ 1.f, -1.f,  1.f},
{-1.f,  1.f,  1.f},
{ 1.f,  1.f,  1.f},
{-1.f, -1.f, -1.f}, /// y
{-1.f,  1.f, -1.f},
{ 1.f, -1.f, -1.f},
{ 1.f,  1.f, -1.f},
{-1.f, -1.f,  1.f},
{-1.f,  1.f,  1.f},
{ 1.f, -1.f,  1.f},
{ 1.f,  1.f,  1.f},
{-1.f, -1.f, -1.f}, /// z
{-1.f, -1.f,  1.f},
{ 1.f, -1.f, -1.f}, 
{ 1.f, -1.f,  1.f},
{-1.f,  1.f, -1.f},
{-1.f,  1.f,  1.f},
{ 1.f,  1.f, -1.f}, 
{ 1.f,  1.f,  1.f}
};

const int ShapeDrawer::MinMaxBoxEdgeV[24][3] = {
{0, 1, 2}, /// x
{3, 1, 2},
{0, 4, 2},
{3, 4, 2},
{0, 1, 5},
{3, 1, 5},
{0, 4, 5},
{3, 4, 5},
{0, 1, 2}, /// y
{0, 4, 2},
{3, 1, 2},
{3, 4, 2},
{0, 1, 5},
{0, 4, 5},
{3, 1, 5},
{3, 4, 5},
{0, 1, 2}, /// z
{0, 1, 5},
{3, 1, 2}, 
{3, 1, 5},
{0, 4, 2},
{0, 4, 5},
{3, 4, 2}, 
{3, 4, 5}
};

void ShapeDrawer::BeginPoints()
{ glBegin(GL_POINTS); }

void ShapeDrawer::BeginLines()
{ glBegin(GL_LINES); }

void ShapeDrawer::End()
{ glEnd(); }

void ShapeDrawer::BeginVertexState()
{ glEnableClientState(GL_VERTEX_ARRAY); }

void ShapeDrawer::EndVertexState()
{ glDisableClientState(GL_VERTEX_ARRAY); }

void ShapeDrawer::Vertex(const float* p)
{ glVertex3fv(p); }

void ShapeDrawer::Line(const float* p, const float* q)
{
    glVertex3fv(p);
    glVertex3fv(q);
}

void ShapeDrawer::BoundingBox(const float* coord)
{
    const float& h = coord[3];
    for(int i=0;i<24;++i) {
        glVertex3f(coord[0] + h * CenterBoxEdgeV[i][0],
                    coord[1] + h * CenterBoxEdgeV[i][1],
                    coord[2] + h * CenterBoxEdgeV[i][2]);
    }
}

void ShapeDrawer::BoundingBoxMinMax(const float* coord)
{
    for(int i=0;i<24;++i) {
        glVertex3f(coord[MinMaxBoxEdgeV[i][0]],
                    coord[MinMaxBoxEdgeV[i][1]],
                    coord[MinMaxBoxEdgeV[i][2]]);
    }
}

void ShapeDrawer::Coordsys(const float& scale)
{
    glBegin(GL_LINES);
    
    glColor3fv(UnitColor[0]);
    glVertex3fv(UnitColor[3]);
    glVertex3f(UnitColor[0][0] * scale, 0.f, 0.f);
    
    glColor3fv(UnitColor[1]);
    glVertex3fv(UnitColor[3]);
    glVertex3f(0.f, UnitColor[1][1] * scale, 0.f);
    
    glColor3fv(UnitColor[2]);
    glVertex3fv(UnitColor[3]);
    glVertex3f(0.f, 0.f, UnitColor[2][2] * scale);
    
    glEnd();
}

void ShapeDrawer::CoordsysArrowed(const float& scale)
{
    const float invscale = 1.f / scale;
    
    Coordsys(scale);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    

    glTranslatef(scale, 0.f, 0.f);
    
    glColor3fv(UnitColor[0]);
    TriangleVertexArray(PyramidV[0], 18, 4);
    
    glTranslatef(-scale, 0.f, 0.f);
    

    glRotatef(90,0,0,1);
    glTranslatef(scale, 0.f, 0.f);
 
    glColor3fv(UnitColor[1]);
    TriangleVertexArray(PyramidV[0], 18, 4);
    
    glTranslatef(-scale, 0.f, 0.f);
    glRotatef(-90,0,0,1);

    glRotatef(-90,0,1,0);
    
    glTranslatef(scale, 0.f, 0.f);

    glColor3fv(UnitColor[2]);
    TriangleVertexArray(PyramidV[0], 18, 4);

    glTranslatef(-scale, 0.f, 0.f);

    glRotatef(90,0,1,0);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    
}

void ShapeDrawer::Arrow(const float* a, const float* b)
{
	glBegin(GL_LINES);
    
    glVertex3fv(a);
    glVertex3fv(b);
    
    glEnd();
	
	Vector3F side(b[0] - a[0], b[1] - a[1], b[2] - a[2]);
	float l = side.length();
	if(l < 1e-3f) {
		throw " ShapeDrawer::Arrow distance too short ";
	}
	side /= l;
    Vector3F front = side.perpendicular();
    Vector3F up = front.cross(side);
    Matrix44F mat;
    mat.setOrientations(side, up, front);
    mat.scaleBy(l * .07f);
    mat.setTranslation(b[0], b[1], b[2]);
    
    glPushMatrix();
    glMultMatrixf((const GLfloat*)&mat);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    
    TriangleVertexArray(PyramidV[0], 18, 4);
	
    glDisableClientState(GL_VERTEX_ARRAY);
    
    glPopMatrix();
}

void ShapeDrawer::Ray(const float* r)
{
    glBegin(GL_LINES);
    
    glVertex3f(r[0] + r[3] * r[6],
                r[1] + r[4] * r[6],
                r[2] + r[5] * r[6]);
    glVertex3f(r[0] + r[3] * r[7],
                r[1] + r[4] * r[7],
                r[2] + r[5] * r[7]);
    
    glEnd();
}

void ShapeDrawer::RayArrowed(const float* r)
{
    Ray(r);
    
    Vector3F side(r[3], r[4], r[5]);
    Vector3F front = side.perpendicular();
    Vector3F up = front.cross(side);
    Matrix44F mat;
    mat.setOrientations(side, up, front);
    mat.scaleBy(.5f);
    mat.setTranslation(r[0] + r[3] * r[6],
                r[1] + r[4] * r[6],
                r[2] + r[5] * r[6]);
    
    glPushMatrix();
    glMultMatrixf((const GLfloat*)&mat);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    
    TriangleVertexArray(PyramidV[0], 18, 4);
    
    glTranslatef((r[7] - r[6])*2.f, 0.f, 0.f);
    
    TriangleVertexArray(PyramidV[0], 18, 4);

    glDisableClientState(GL_VERTEX_ARRAY);
    
    glPopMatrix();
}

void ShapeDrawer::TriangleVertexElement(const float* p, const int* ind,
                    int n, int k)
{
    glVertexPointer(k, GL_FLOAT, 0, (GLfloat*)p);
    glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_INT, ind);
}

void ShapeDrawer::TriangleVertexArray(const float* p, int n, int k)
{
    glVertexPointer(k, GL_FLOAT, 0, (GLfloat*)p);
    glDrawArrays(GL_TRIANGLES, 0, n);
}

void ShapeDrawer::PointVertexArray(const float* p, int n, int k)
{
	glVertexPointer(k, GL_FLOAT, 0, (GLfloat*)p);
    glDrawArrays(GL_POINTS, 0, n);
}

void ShapeDrawer::ColorGrid(const float* u, const float* c, int d)
{
	const int d1 = d + 1;
	const int d2 = d1*d1;
	for(int k=0;k<d1;++k) {
		for(int j=0;j<d1;++j) {
			for(int i=0;i<d1;++i) {
				
				int v1 = 3*(k*d2 + j* d1 + i);
				glColor3fv(&c[v1]);
				glVertex3fv(&u[v1]);
				
			}
		}
	}
}

void ShapeDrawer::SetColor(float r, float g, float b)
{ glColor3f(r, g, b); }

void ShapeDrawer::LightRed()
{ glColor3f(.8f, .17f, .2f); }

void ShapeDrawer::LightGreen()
{ glColor3f(.03f, .7f, .07f); }

void ShapeDrawer::LightBlue()
{ glColor3f(.03f, .2f, .7f); }

void ShapeDrawer::LightYellow()
{ glColor3f(.7f, .57f, 0.f); }

void ShapeDrawer::DarkRed()
{ glColor3f(.2f, .08f, .03f); }

void ShapeDrawer::DarkGreen()
{ glColor3f(.03f, .2f, .08f); }

void ShapeDrawer::DarkBlue()
{ glColor3f(.03f, .08f, .2f); }

void ShapeDrawer::BeginSpace(const float* mat)
{
	glPushMatrix();
	glMultMatrixf(mat);
}

void ShapeDrawer::EndSpace()
{ glPopMatrix(); }

}