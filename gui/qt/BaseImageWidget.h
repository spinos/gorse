/*
 *  2D view with track and zoom
 *	aloe
 */
 
#ifndef ALO_IMAGE_WIDGET_H
#define ALO_IMAGE_WIDGET_H

#include <QPixmap>
#include <QImage>
#include <QWidget>

namespace alo {

class BaseImageWidget : public QWidget
{
    Q_OBJECT

public:
    BaseImageWidget(QWidget *parent = 0);

	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);
	void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
	virtual QSize	minimumSizeHint() const;
	
/// horizontal and vertical margin size
	void setMargin(const int & h, const int & v);
	
protected:
	virtual void clientDraw(QPainter * pr);
	virtual QColor backgroundCol() const;
	
	const QSize & portSize() const; 
	bool isLandscape() const;
	const QSize & margin() const;
	
	virtual void processSelect(QMouseEvent *event);
	virtual void processDeselect(QMouseEvent *event);
	virtual void processMouseInput(QMouseEvent *event);
	virtual void processCamera(QMouseEvent *event);
	
private:
	void trackCamera(int dx, int dy);
	void zoomCamera(int dx);
	
private:

	QSize m_margin;
	QSize m_portSize;
	QPoint m_lastMousePos;
	QPointF m_translation, m_scaling;
	
};

}
#endif