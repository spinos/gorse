#ifndef DRAWABLE_SCENE_H
#define DRAWABLE_SCENE_H

#include <QMutex>
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
    std::deque<DrawableObject *> m_editQueue;
    std::deque<DrawableObject *> m_removeQueue;
    std::vector<DrawableObject *> m_drawables;
    QOpenGLContext *m_ctx;
    QMutex m_mutex;
    
public:
    DrawableScene();
    ~DrawableScene();

    void setContext(QOpenGLContext *x);

    void cleanup();

    void initializeScene();
    void draw(const QMatrix4x4 &proj, const QMatrix4x4 &cam);

    DrawableObject *createDrawable();
    void enqueueEditDrawable(DrawableObject *d);
    void enqueueRemoveDrawable(DrawableObject *d);

    QOpenGLContext *context();
    
    void lock();
    void unlock();

private:
    bool removeDrawable(DrawableObject *k);

};

}

#endif
