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

QT_BEGIN_NAMESPACE
class QJsonObject;
QT_END_NAMESPACE

namespace alo {
class TreeModel;

template<typename T>
class GroupCollection;

}

class AssetPalette : public QWidget
{
	Q_OBJECT
	
public:
	AssetPalette(QWidget *parent = 0);

	alo::GroupCollection<QJsonObject> *assetCollector();
	
protected:

signals:
	
public slots:

private slots:
	
private:
	
	alo::TreeModel *m_model;

};
#endif