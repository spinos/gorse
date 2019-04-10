/* window.h
 * envt
 *
 * environment test
 */

#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE

QT_END_NAMESPACE

class RenderWidget;
class SampleDlg;
class ImgDlg;

class Window : public QMainWindow
{
    Q_OBJECT

public:
    Window(const char* filename);

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    RenderWidget* m_view;
	SampleDlg* m_envs;
	ImgDlg* m_img;
	
};

#endif
