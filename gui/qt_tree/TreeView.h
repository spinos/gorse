#ifndef GAR_TREE_VIEW_H
#define GAR_TREE_VIEW_H

#include <QTreeView>


class TreeView : public QTreeView
{
	Q_OBJECT
	
public:
	TreeView(QWidget *parent = 0);
	
protected:

	virtual void mousePressEvent(QMouseEvent *event) override;

signals:
	
	void onItemSelected(int);
	
public slots:
	
private slots:

private:

};

#endif