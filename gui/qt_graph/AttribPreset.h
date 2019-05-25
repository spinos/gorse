/*
 *  AttribPreset.h
 *  gorse
 *
 *  preset from a json file
 *
 *  2019/5/23
 */
 
#ifndef GORS_ATTRIB_PRESET_H
#define GORS_ATTRIB_PRESET_H

#include <QJsonObject>

namespace alo {
    
class AttribPreset {
    
public:

    static void loadAttributePreset(const QString &fileName);
    
protected:
    
    QJsonObject getTransformPresetObj(bool &found);
    
    static QJsonObject attributePresetObj;
    
private:
    
};
    
}

#endif
