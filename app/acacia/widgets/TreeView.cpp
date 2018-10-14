#include "TreeView.h"
#include <QDebug>
#include <QMouseEvent>

TreeView::TreeView(QWidget *parent) : QTreeView(parent)
{}

void TreeView::mousePressEvent(QMouseEvent *event) 
{
	QTreeView::mousePressEvent(event);
	QModelIndex item = indexAt(event->pos());

	QVariant clicked = model()->data(item, Qt::UserRole);
	emit onItemSelected(clicked.toInt());

}