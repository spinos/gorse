/*
 *  gl_heads.h
 *  aloe
 *
 *  Created by jian zhang on 9/30/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
 
#ifndef ALO_GL_HEADS_H
#define ALO_GL_HEADS_H

#ifdef WIN32
//#include <ogl/gExtension.h>
//#include <windows.h>
//#include <GL/gl.h>
//#include <GL/glu.h>
#include <GL/glew.h>

//#include <QtANGLE/KHR/khrplatform.h>
//#include <QtANGLE/GLES2/gl2.h>
//#include <QtANGLE/GLES2/gl2ext.h>
/*
#include <cstdint>
#ifndef GL_HEADER_HAS_GLINT64

    typedef int64_t khronos_int64_t;
#endif
#ifndef GL_HEADER_HAS_GLUINT64

    typedef uint64_t khronos_uint64_t;
#endif
*/
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>
#endif

#ifdef LINUX
#include <GL/glew.h>
#include <GL/gl.h>
#endif

#endif


