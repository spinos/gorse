#include <QtGui>

#include "window.h"
#include "instdwidget.h"

using namespace alo;

Window::Window()
{
    m_plot = new BrdfWidget(this);
	
	setCentralWidget(m_plot);
    setWindowTitle(tr("Instanced Draw"));
	
}

void Window::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
        close();

	QWidget::keyPressEvent(e);
}
