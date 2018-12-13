/*
 *  WireframeProgram.h
 *  aloe
 *
 */

#ifndef OGL_WIREFRAME_PROGRAM_H
#define OGL_WIREFRAME_PROGRAM_H

#include "BaseProgram.h"

namespace alo {

class WireframeProgram : public BaseProgram
{
    int m_wireColorLoc;
    int m_surfaceColorLoc;
    
public:
    WireframeProgram();
    virtual ~WireframeProgram();
    
    virtual void initializeProgram(QOpenGLContext *ctx) override;
    virtual void setWireColor(const float *c) override;
    virtual void setSurfaceColor(const float *c) override;
    
protected:
    
private:
    
};

}

#endif
