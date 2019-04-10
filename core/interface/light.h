/*
 *  light.h
 *  aloe interface
 *
 *  Created by jian zhang on 3/28/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_INTERFACE_LIGHT_H
#define ALO_INTERFACE_LIGHT_H

namespace alo {

class Vector3F;

template<typename T>
class EnvironmentLight;

typedef EnvironmentLight<Vector3F> EnvLightTyp;

}

#endif
