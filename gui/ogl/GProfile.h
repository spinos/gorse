/*
 *  GProfile.h
 *  aloe
 *
 *  Created by jian zhang on 9/18/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_G_PROFILE_H
#define ALO_G_PROFILE_H

namespace alo {

class GMaterial;

class GProfile {
public:
	GProfile();
		
	GProfile(bool lighting, bool depthTest, bool wired, bool culled, bool textured);	
	void apply() const;
	
	GMaterial * m_material;
	bool m_lighting, m_depthTest, m_wired, m_culled, m_textured;
};

class MarkerProfile : public GProfile {

public:
    
    MarkerProfile();
    
};

class WireProfile : public GProfile {

public:
    
    WireProfile();
    
};

}
#endif