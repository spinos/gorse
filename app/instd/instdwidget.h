/*
 *  brdft
 *  aloe
 */
#ifndef BRDF_WIDGET_H
#define BRDF_WIDGET_H

#include <qt_ogl/Base3DView.h>
#include <math/Vector3F.h>

namespace alo {
class WireProfile;
class MarkerProfile;
}

class BrdfWidget : public alo::Base3DView
{
    Q_OBJECT

public:
    BrdfWidget(QWidget *parent = 0);
    ~BrdfWidget();

protected:
    virtual void clientInit();
    virtual void clientDraw();
	virtual void clientSelect(QMouseEvent *event);
    virtual void clientDeselect(QMouseEvent *event);
    virtual void clientMouseInput(QMouseEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);
	
public slots:

signals:

private:
	void computeHammersley();
    
	alo::WireProfile* m_wireProf;
    alo::MarkerProfile* m_markerProf;
    
};

#endif
