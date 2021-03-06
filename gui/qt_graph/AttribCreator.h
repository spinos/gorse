/*
 *  AttribCreator.h
 *  gorse
 *
 *  create attributes from content and add to ops
 *
 *  2019/5/22
 */

#ifndef GORS_ATTRIB_CREATOR_H
#define GORS_ATTRIB_CREATOR_H

#include "AttribPreset.h"

namespace alo {

class QAttrib;
class GlyphOps;

class AttribCreator : public AttribPreset {

public:

	void addAttributes(GlyphOps *ops, const QJsonObject &content);

private:
	void addAttribute(GlyphOps *ops, const QJsonObject &content);
	QAttrib *createBoolAttribute(const QJsonObject &content);
	QAttrib *createIntAttribute(const QJsonObject &content);
	QAttrib *createFloatAttribute(const QJsonObject &content);
	QAttrib *createFloat2Attribute(const QJsonObject &content);
	QAttrib *createFloat3Attribute(const QJsonObject &content);
	QAttrib *createMeshAttribute(const QJsonObject &content);
	QAttrib *createListAttribute(const QJsonObject &content);
	QAttrib *createStringAttribute(const QJsonObject &content);
	void addTransformAttributes(GlyphOps *ops);
	void addConnection(QAttrib *b, const QJsonObject &content);

};

}

#endif
