#ifndef ALO_GL_HUD_H
#define ALO_GL_HUD_H

#include "gl_heads.h"
#include <string>

namespace alo {

class BaseCamera;
class GLHUD {
public:
    GLHUD();
    virtual ~GLHUD();
    void reset();
    void setCamera(BaseCamera * cam);
    void drawString(const std::string & str, const int & row) const;
private:
    static GLuint m_texture;
    BaseCamera * m_camera;
};

}

#endif
