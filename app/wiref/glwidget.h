#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <qt_ogl/View3DWidget.h>
#include <qt_ogl/DrawableScene.h>
#include <math/SimpleBuffer.h>
#include <math/Vector3F.h>

class GLWidget : public alo::View3DWidget
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();

public slots:
    void cleanup();

signals:

protected:
    virtual void clientInit();
    virtual void clientDraw(const QMatrix4x4 &proj, const QMatrix4x4 &cam);
    virtual void keyPressEvent(QKeyEvent *event);

private:

	void addCylinder();

	alo::DrawableScene *m_scene;
	alo::SimpleBuffer<alo::Vector3F> posarr;
    alo::SimpleBuffer<alo::Vector3F> nmlarr;
    alo::SimpleBuffer<alo::Vector3F> baryc;
};

#endif
