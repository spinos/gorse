/*
 *  brdft
 *  aloe
 */
#include <QtGui>
#include <QtOpenGL>
#include "instdwidget.h"
#include <ogl/GProfile.h>
#include <ogl/ShapeDrawer.h>
#include <rng/Lehmer.h>
#include <rng/Uniform.h>
#include <rng/Hammersley.h>
#include <ctime>

using namespace alo;

BrdfWidget::BrdfWidget(QWidget *parent)
    : Base3DView(parent)
{ 
	usePerspCamera(); 
    m_wireProf = new WireProfile;
    m_markerProf = new MarkerProfile;

}

BrdfWidget::~BrdfWidget()
{}

void BrdfWidget::clientInit()
{
	Vector3F nml(.7f, 1.8f, .37f);
	nml.normalize();

	Vector3F eyed(1.f, -.9f, -.5f);
	eyed.normalize();

	std::time_t secs = std::time(0);
	Uniform<Lehmer> lmlcg(secs);

}

void BrdfWidget::clientDraw()
{
	m_markerProf->apply();
	ShapeDrawer::CoordsysArrowed(20.f);

	glScalef(20,20,20);
	m_wireProf->apply();
	
	float mat[16] = {0.f, 0.f, 0.f, 0.f,
					0.f, 0.f, 0.f, 0.f,
					0.f, 0.f, 0.f, 0.f,
					0.f, 0.f, 0.f, 1.f};

	ShapeDrawer::BeginSpace(mat);
    ShapeDrawer::DarkGreen();
	ShapeDrawer::BeginVertexState();
	
    //ShapeDrawer::TriangleVertexElement((const float*)m_shape->c_points(),
    //                m_shape->c_indices(), m_shape->numIndices(), 3);
					
    ShapeDrawer::EndVertexState();
	ShapeDrawer::EndSpace();
	
	m_markerProf->apply();
	
	ShapeDrawer::LightBlue();
			
	ShapeDrawer::End();
	
#if 0
	ShapeDrawer::LightYellow();
	ShapeDrawer::BeginPoints();
	for(int i=0;i<8192;++i) {
		q.set(m_hamm[i][0], m_hamm[i][1], 0.f);
		ShapeDrawer::Vertex((const float*)&q);
	}
	ShapeDrawer::End();
#endif

}

void BrdfWidget::clientSelect(QMouseEvent *event)
{
	update();
}

void BrdfWidget::clientDeselect(QMouseEvent *event)
{
	update();
}

void BrdfWidget::clientMouseInput(QMouseEvent *event)
{
	update();
}

void BrdfWidget::keyPressEvent(QKeyEvent *e)
{
	switch (e->key()) {
		case Qt::Key_M:
			break;
		case Qt::Key_L:
			break;
		case Qt::Key_P:
			break;
		default:
			break;
	}
	Base3DView::keyPressEvent(e);
}

