#ifndef VCHL_ATTRIB_EDITOR_H
#define VCHL_ATTRIB_EDITOR_H

#include <QWidget>
#include <QQueue>
#include <string>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QSpacerItem;
class QListWidgetItem;
QT_END_NAMESPACE

namespace alo {
class GlyphOps;
class QAttrib;
class GlyphScene;
}

class AttribEditor : public QWidget
{
    Q_OBJECT

public:
	AttribEditor(alo::GlyphScene *scene, QWidget *parent = 0);
	
protected:

signals:
	void sendAttribChanged();

public slots:
	void recvSelectGlyph(bool isSelecting);
	
private slots:
	void recvIntValue(QPair<std::string, float> x);
	void recvFloatValue(QPair<std::string, float> x);
	void recvListItemSelection(QPair<std::string, std::string> x);
	void recvBoolValue(QPair<std::string, bool> x);
	void recvStringValue(QPair<std::string, std::string> x);
	void recvSplitMove(int pos, int index);
	void recvBeginEditing();
    void recvEndEditing();
    
private:
	void clearAttribs();
	void lsAttribs(alo::GlyphOps *ops);
	void lsAttr(alo::QAttrib *attr);
	void lsIntAttr(alo::QAttrib *attr);
	void lsFloatAttr(alo::QAttrib *attr);
	void lsFloat2Attr(alo::QAttrib *attr);
	void lsFloat3Attr(alo::QAttrib *attr);
	void lsListAttr(alo::QAttrib *attr);
	void lsBoolAttr(alo::QAttrib *attr);
	void lsStringAttr(alo::QAttrib *attr);
	
private:
	alo::GlyphScene *m_scene;
	QQueue<QWidget *> m_leftCollWigs;
	QQueue<QWidget *> m_rightCollWigs;
	QVBoxLayout *leftBox;
	QVBoxLayout *rightBox;
	QSpacerItem *m_leftSpace;
	QSpacerItem *m_rightSpace;

};

#endif
