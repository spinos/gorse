
/*
 *  PiecesList.h
 *  vachellia
 *
 *  Created by jian zhang on 3/30/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef VCHL_PIECES_LIST_H
#define VCHL_PIECES_LIST_H

#include <QListWidget>

namespace alo {
class TreeModel;
}

class PiecesList : public QListWidget
{
    Q_OBJECT

public:
    PiecesList(alo::TreeModel *model, QWidget *parent = 0);

public slots:
    void selectItem(int id);
	
protected:
	void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void startDrag(Qt::DropActions supportedActions);
	
private:

	alo::TreeModel *m_model;
};

#endif