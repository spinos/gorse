/* window.cpp
 * envt
 *
 */

#include <QtGui>
#include "window.h"
#include "RenderWidget.h"
#include "SampleDlg.h"
#include "ImgDlg.h"
#include "EnvironScene.h"

Window::Window(const char* filename)
{
    m_view = new RenderWidget(this);
	m_view->readImage(filename);
	m_view->setImageExposure(1.2f);
	m_envs = new SampleDlg(this);
	const alo::Scene* scn = m_view->scene();
	const alo::EnvLightTyp* lit = scn->environmentLight();
	m_envs->setEnvLight(lit);
	m_img = new ImgDlg(this);
	m_img->setEnvLight(lit);
	
	setCentralWidget(m_view);
    setWindowTitle(tr("Environment + Lambert"));
	
	m_envs->show();
	m_img->show();
}

void Window::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
        close();

	QWidget::keyPressEvent(e);
}
