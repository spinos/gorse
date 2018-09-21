#ifndef BRDF_WINDOW_H
#define BRDF_WINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE

QT_END_NAMESPACE

class BrdfWidget;

class Window : public QMainWindow
{
    Q_OBJECT

public:
    Window();

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    BrdfWidget* m_plot;
	
};

#endif
