/*
 *  GMaterial.h
 *  aloe
 *
 *  Created by jian zhang on 9/18/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_G_MATERIAL_H
#define ALO_G_MATERIAL_H

namespace alo {

class GMaterial {
public:
	GMaterial();		
	GMaterial(const float* ambient,
             const float* frontDiffuse,
			 const float* backDiffuse,
             const float* specular,
             const float* emission,
             float shininess);
 
    void apply() const;
    void applyColor() const;
 
    float m_Ambient[4];
    float m_Diffuse[4];
	float m_backDiffuse[4];
    float m_Specular[4];
    float m_Emission[4];
    float  m_Shininess;
};

}
#endif
