/*
 *  AttribPreset.cpp
 *  gorse
 *
 *  2019/5/23
 */

#include "AttribPreset.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QDebug>

namespace alo {

QJsonObject AttribPreset::attributePresetObj;

void AttribPreset::loadAttributePreset(const QString &fileName)
{
	QFile loadFile(fileName);
	if (!loadFile.open(QIODevice::ReadOnly) ) {
		qWarning("Couldn't open file for attribute preset");
		return;
	}

	QByteArray loadData = loadFile.readAll();
	QJsonDocument loadDoc(QJsonDocument::fromJson(loadData));
	
	attributePresetObj = loadDoc.object();
}

QJsonObject AttribPreset::getTransformPresetObj(bool &found)
{
	found = false;
	QJsonArray attrArray = attributePresetObj["children"].toArray();
	QJsonObject res;
	for(int i=0;i<attrArray.size();++i) {
		res = attrArray[i].toObject();
		if(res["name"].toString() == "transform") {
			found = true;
			break;
		}
	}
	
	return res;
}

} /// end of alo
