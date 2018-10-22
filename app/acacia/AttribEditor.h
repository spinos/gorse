#ifndef GAR_ATTRIB_EDITOR_H
#define GAR_ATTRIB_EDITOR_H

#include <QWidget>
#include <QQueue>
#include <string>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QSpacerItem;
QT_END_NAMESPACE

namespace alo {
class GlyphOps;
class QAttrib;
}

class AcaciaScene;

class AttribEditor : public QWidget
{
    Q_OBJECT

public:
	AttribEditor(AcaciaScene *scene, QWidget *parent = 0);

signals:
	void sendAttribChanged();

public slots:
	void recvSelectGlyph(bool isSelecting);

private slots:
	void recvFloatValue(QPair<std::string, float> x);

private:
	void clearAttribs();
	void lsAttribs(alo::GlyphOps *ops);
	void lsAttr(alo::QAttrib *attr);
	void lsFloatAttr(alo::QAttrib *attr);
	void lsFloat2Attr(alo::QAttrib *attr);

private:
	AcaciaScene *m_scene;
	QQueue<QWidget *> m_leftCollWigs;
	QQueue<QWidget *> m_rightCollWigs;
	QVBoxLayout *leftBox;
	QVBoxLayout *rightBox;
	QSpacerItem *m_leftSpace;
	QSpacerItem *m_rightSpace;

};

#endif
