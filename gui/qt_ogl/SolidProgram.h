/*
 *  SolidProgram.h
 *  aloe
 *
 */

#ifndef OGL_SOLID_PROGRAM_H
#define OGL_SOLID_PROGRAM_H

#include "BaseProgram.h"

namespace alo {

class SolidProgram : public BaseProgram
{
    int m_lightPosLoc;
    int m_surfaceColorLoc;

public:
    SolidProgram();
    virtual ~SolidProgram();
    
    virtual void initializeProgram(QOpenGLContext *ctx) override;
    virtual void setWireColor(const float *c) override;
    virtual void setSurfaceColor(const float *c) override;
    
protected:
    
private:
    
};

}

#endif
