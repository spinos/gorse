/*
 *  PiecesList.cpp
 *  garden
 *
 *  Created by jian zhang on 3/30/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#include <QtGui>
#include "PiecesList.h"
#include <QPixmap>
#include <QVariant>
#include <qt_tree/treemodel.h>

using namespace alo;

PiecesList::PiecesList(TreeModel *model, QWidget *parent)
    : QListWidget(parent)
{
	m_model = model;
    setDragEnabled(true);
    setViewMode(QListView::IconMode);
    setIconSize(QSize(64, 64));
	setGridSize(QSize(80, 100));
    //setSpacing(12);
    setAcceptDrops(false);
    setDropIndicatorShown(false);
	//setMaximumHeight(48);
}

void PiecesList::dragEnterEvent(QDragEnterEvent *event)
{
	qDebug()<<"PiecesList::dragEnterEvent";
	if (event->mimeData()->hasFormat("application/x-dndpiecedata")) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    } else
        event->ignore();
}

void PiecesList::dragMoveEvent(QDragMoveEvent *event)
{
	qDebug()<<"PiecesList::dragMoveEvent";
	if (event->mimeData()->hasFormat("application/x-dndpiecedata"))
        event->accept();
    else
        event->ignore();
}

void PiecesList::startDrag(Qt::DropActions /*supportedActions*/)
{
	QListWidgetItem *item = currentItem();

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    QVariant varpix = item->data(Qt::UserRole);
    QPixmap pixmap = varpix.value<QPixmap>();
    int pieceTyp = item->data(Qt::UserRole+1).toInt();

    dataStream << pixmap << pieceTyp;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-dndpiecedata", itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));
    drag->setPixmap(pixmap);

	qDebug()<<"PiecesList::startDrag"<<"pixmap"<<pixmap<<" id "<<pieceTyp;
	
	drag->exec(Qt::MoveAction);
	setCursor(Qt::OpenHandCursor);
}

void PiecesList::selectItem(int id)
{
	QListWidget::clear();
	const QList<QJsonObject> elems = m_model->listElements(id);
	QList<QJsonObject>::const_iterator it = elems.begin();
	for(;it!=elems.end();++it) {

		const QJsonObject& elemObj = *it;
		
		QListWidgetItem *pieceItem = new QListWidgetItem(this);

		int pieceId = elemObj["id"].toInt(); ;
		QString pieceLabel = elemObj["label"].toString();
		QPixmap pixm(":/images/unknown.png");

		QIcon cloverIcon(pixm);
		pieceItem->setIcon(cloverIcon);
		pieceItem->setText(pieceLabel);
		pieceItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable
							| Qt::ItemIsDragEnabled);
		pieceItem->setData(Qt::UserRole, QVariant(pixm) );
		pieceItem->setData(Qt::UserRole+1, pieceId);
	}
}
