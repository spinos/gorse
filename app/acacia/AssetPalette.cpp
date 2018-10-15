/*
 *  AssetPalette.cpp
 *  
 *
 *  Created by jian zhang on 3/22/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#include <QtWidgets>
#include "AssetPalette.h"
#include "PiecesList.h"
#include <qt_tree/TreeView.h>
#include <qt_tree/treemodel.h>

using namespace alo;

AssetPalette::AssetPalette(QWidget *parent) : QWidget(parent)
{
	QFile loadFile(":/mimes/assettree.json");
	if (!loadFile.open(QIODevice::ReadOnly) ) {
		qWarning("Couldn't open file assettree");
	}
	QByteArray loadData = loadFile.readAll();
	QJsonDocument loadDoc(QJsonDocument::fromJson(loadData));
	
	m_model = new TreeModel(loadDoc.object());

    TreeView* view = new TreeView(this);
    view->setModel(m_model);
    view->setColumnWidth(0, 80);
    view->setHeaderHidden(true);
    view->setColumnHidden(2, true);

    PiecesList *pieces = new PiecesList(m_model, this);

    QSplitter *splitter = new QSplitter(Qt::Vertical, this);
    splitter->addWidget(view);
    splitter->addWidget(pieces);
	
	QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(splitter);
    mainLayout->setContentsMargins(0,0,0,0);
	setLayout(mainLayout);
	
	connect(view, SIGNAL(onItemSelected(int) ), 
		pieces, SLOT(selectItem(int) ) );

}
