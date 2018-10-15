/*
 *  AssetPalette.h
 *  garden
 *
 *  Created by jian zhang on 3/22/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef GAR_ASSET_PALETTE_H
#define GAR_ASSET_PALETTE_H

#include <QWidget>

namespace alo {
class TreeModel;
}

class AssetPalette : public QWidget
{
	Q_OBJECT
	
public:
	AssetPalette(QWidget *parent = 0);
	
protected:

signals:
	
public slots:

private slots:
	
private:
	
	alo::TreeModel *m_model;

};
#endif