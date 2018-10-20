#ifndef DRAWABLE_SCENE_H
#define DRAWABLE_SCENE_H

#include <QMatrix4x4>
#include <vector>
#include <deque>

QT_FORWARD_DECLARE_CLASS(QOpenGLContext)

namespace alo {

class WireframeProgram;
class DrawableObject;

class DrawableScene
{
	WireframeProgram *m_program1;
    std::deque<DrawableObject *> m_createQueue;
    std::vector<DrawableObject *> m_drawables;
    QOpenGLContext *m_ctx;

public:
    DrawableScene();
    ~DrawableScene();

    void setContext(QOpenGLContext *x);

    void cleanup();

    void initializeScene();
    void draw(const QMatrix4x4 &proj, const QMatrix4x4 &cam);

    DrawableObject *createDrawable();
    bool removeDrawable(DrawableObject *k);

    QOpenGLContext *context();

private:

};

}

#endif
