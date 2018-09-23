/*
 *  GLSLBase.cpp
 *  pmap
 *
 *  Created by jian zhang on 10/31/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "GlslBase.h"
#include <sstream>

namespace alo {

typedef struct glExtensionEntry {
    char* name;
    GLfloat promoted;
    GLboolean supported;
} glExtensionEntry;

glExtensionEntry entriesNeeded[] = {
{"GL_EXT_framebuffer_object",   0.0, 0},
{"GL_ARB_texture_cube_map",     1.3, 0},
{"GL_ARB_shader_objects",       2.0, 0},
{"GL_ARB_shading_language_100", 2.0, 0},
{"GL_ARB_fragment_shader",      2.0, 0},
{"GL_ARB_vertex_buffer_object",      2.0, 0},
{"GL_ARB_multitexture",      1.3, 0},
{"GL_ARB_multisample",      1.3, 0},
{"GL_ARB_vertex_program",      2.0, 0},
{"GL_ARB_fragment_program",      2.0, 0},
{"GL_ARB_texture_rectangle",      0.0, 0},
};

float GLSLBase::CoreVersion = 0.f;

GLSLBase::GLSLBase() :  
m_hasShaders(0), 
fHasFBO(0),
fPixels(0)
{}

GLSLBase::~GLSLBase() 
{
	if (program_object) {
		glDeleteObjectARB(program_object);
	}
		
	if(fPixels) {
		delete[] fPixels;
	}
}

const char* GLSLBase::vertexProgramSource() const
{
	return "void main()"
"{"
"		gl_Position = ftransform();"
"		gl_FrontColor = gl_Color;"
"}";
}

const char* GLSLBase::fragmentProgramSource() const
{
	return "void main()"
"{"
"		gl_FragColor = gl_Color * vec4 (0.25);"
"}";
}

bool GLSLBase::diagnose(std::string& log)
{
	if(isDiagnosed() ) {
		return true;
	}
	
	std::string sver((char *)glGetString(GL_VERSION));
	std::stringstream sst(sver);
	sst>>CoreVersion;
	
	sst.str("");
	sst<<"\n GL_Version "<<sver
	    <<"\n GL_Renderer "<<((char *)glGetString(GL_RENDERER));
	
	log = sst.str();
	
	if(CoreVersion < 1.4) {
		log += " ERROR OpenGL version too low, this thing may not work correctly!\n";
	}
	
	log += "\n support routines: ";
	
	int supported = 1;
	int j = sizeof(entriesNeeded)/sizeof(glExtensionEntry);
	
#ifdef XWIN32
	gExtensionInit();
     char sbuf[64];
	 for (int i = 0; i < j; i++) {
		 if(!gCheckExtension(entriesNeeded[i].name)) {
			 sprintf(sbuf, "%-32s %d\n", entriesNeeded[i].name, 0);
			 supported = 0;
		 }
		else sprintf(sbuf, "%-32s %d\n", entriesNeeded[i].name, 1);
		log += sbuf;
	}
#else

#ifdef __APPLE__

#else
    glewInit();
#endif
/*
	const GLubyte *strExt = glGetString(GL_EXTENSIONS);
	for (int i = 0; i < j; i++) {
		entriesNeeded[i].supported = gluCheckExtension((GLubyte*)entriesNeeded[i].name, strExt) |
		(entriesNeeded[i].promoted && (CoreVersion >= entriesNeeded[i].promoted));
		// sprintf(sbuf, "%-32s %d\n", entriesNeeded[i].name, entriesNeeded[i].supported);
		sst.str("");
		sst<<"\n "<<entriesNeeded[i].name<<" "<<(int)entriesNeeded[i].supported;
		log += sst.str();
		supported &= entriesNeeded[i].supported;
	}*/
#endif	
	
	if(supported != 1) return 0;

	log += "\n OpenGL diagnosed";
	
	return 1;
}

bool GLSLBase::isDiagnosed() 
{ return CoreVersion >= 2.0; }

char GLSLBase::initializeShaders(std::string& log)
{
    log = "";
	GLint vertex_compiled, fragment_compiled;
	GLint linked;
		
// Delete any existing program object 
	if (program_object) {
		glDeleteObjectARB(program_object);
		program_object = NULL;
	}
		
	vertex_shader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	const char* a = vertexProgramSource();
	glShaderSourceARB(vertex_shader, 1, &a, NULL);
	glCompileShaderARB(vertex_shader);
	glGetObjectParameterivARB(vertex_shader, GL_OBJECT_COMPILE_STATUS_ARB, &vertex_compiled);
	
	fragment_shader   = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	const char* b = fragmentProgramSource();
	glShaderSourceARB(fragment_shader, 1, &b, NULL);
	glCompileShaderARB(fragment_shader);
	glGetObjectParameterivARB(fragment_shader, GL_OBJECT_COMPILE_STATUS_ARB, &fragment_compiled);
	
	if (!vertex_compiled || !fragment_compiled) {
		if (vertex_shader) {
			glDeleteObjectARB(vertex_shader);
			vertex_shader   = NULL;
			log += "\n Glsl error vertex shader not compiled \n";
		}
		if (fragment_shader) {
			glDeleteObjectARB(fragment_shader);
			fragment_shader = NULL;
			log += "\n Glsl error fragment shader not compiled \n";
		}
		return 0;
	}
	
	program_object = glCreateProgramObjectARB();
	if (vertex_shader != NULL)
	{
		glAttachObjectARB(program_object, vertex_shader);
		glDeleteObjectARB(vertex_shader);
	}
	if (fragment_shader != NULL)
	{
		glAttachObjectARB(program_object, fragment_shader);
		glDeleteObjectARB(fragment_shader);
	}
	glLinkProgramARB(program_object);
	glGetObjectParameterivARB(program_object, GL_OBJECT_LINK_STATUS_ARB, &linked);
		
	if (!linked) {
		glDeleteObjectARB(program_object);
		program_object = NULL;
		log += "\n Glsl error shaders not linked";
		return 0;
	}

	defaultShaderParameters();
	log += "\n Glsl shaders compiled";
	m_hasShaders = 1;
	return 1;
}

char GLSLBase::hasShaders() const
{ return m_hasShaders; }

void GLSLBase::defaultShaderParameters()
{}

void GLSLBase::updateShaderParameters() const
{}

GLhandleARB * GLSLBase::program()
{ return &program_object; }

void GLSLBase::programBegin() const
{
	glUseProgramObjectARB(program_object);
	updateShaderParameters();
}

void GLSLBase::programEnd() const
{
	glUseProgramObjectARB(NULL);
}

char GLSLBase::hasFBO() const 
{ return fHasFBO; }

char GLSLBase::initializeFBO(std::string& log)
{
	glGenFramebuffersEXT(1, &fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

// Create the render buffer for depth	
	glGenRenderbuffersEXT(1, &depthBuffer);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBuffer);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, frameBufferWidth(), frameBufferHeight());

// Now setup the first texture to render to
	glGenTextures(1, &img);
	glBindTexture(GL_TEXTURE_2D, img);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, frameBufferWidth(), frameBufferHeight(), 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

// And attach it to the FBO so we can render to it
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, img, 0);
	
// Attach the depth render buffer to the FBO as it's depth attachment
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthBuffer);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(status != GL_FRAMEBUFFER_COMPLETE_EXT) {
		log += "cannot create frame buffer object";
		fHasFBO = 0;
		return 0;
	}
	
// Unbind the FBO for now
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	fHasFBO = 1;
	
	fPixels = new float[frameBufferWidth() * frameBufferHeight() * 4];
	return 1;
}

int GLSLBase::frameBufferWidth() const
{ return 512; }

int GLSLBase::frameBufferHeight() const
{ return 512; }

void GLSLBase::frameBufferBegin() const
{
// First we bind the FBO so we can render to it
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

// Save the view port and set it to the size of the texture
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glViewport(0,0, frameBufferWidth(), frameBufferHeight());
	
// Then render as normal
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glClearDepth(1.0f);		  
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	
}

void GLSLBase::frameBufferEnd() const
{
	glPopAttrib();
	if(fPixels ) glReadPixels( 0, 0, frameBufferWidth(), frameBufferHeight(), GL_RGBA, GL_FLOAT, fPixels);
// unbind fbo
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void GLSLBase::showFrameBuffer() const
{
	if(!fPixels)
		return;
		
	glDrawPixels(frameBufferWidth(), frameBufferHeight(),GL_RGBA,GL_FLOAT, fPixels);
}

void GLSLBase::drawFrameBuffer()
{}

const float * GLSLBase::pixels() const
{ return fPixels; }
}

