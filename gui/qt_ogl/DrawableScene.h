/*
 *  DrawableScene.h
 *  aloe
 *
 *  draw queue 
 *  index (object_id, group_id)
 *  value object
 *  create, edit, destroy
 *
 */

#ifndef DRAWABLE_SCENE_H
#define DRAWABLE_SCENE_H

#include <QMutex>
#include <QMatrix4x4>
#include <sdb/Types.h>
#include <sdb/L3Tree.h>

QT_FORWARD_DECLARE_CLASS(QOpenGLContext)

namespace alo {

class WireframeProgram;
class DrawableObject;

class DrawableScene
{
	WireframeProgram *m_program1;
    QOpenGLContext *m_ctx;
    QMutex m_mutex;

    enum ObjectState {
        stUnknown = 0,
        stDestroyed,
        stWaitDestroy,
        stHidden,
        stWaitEdit,
        stWaitCreate,
        stNormal
    };

    struct DrawObjectState {
        ObjectState _state;
        DrawableObject *_object;
    };

    sdb::L3Tree<sdb::Coord2, DrawObjectState, 2048, 512, 1024 > m_drawQueue;
    typedef sdb::L3Node<sdb::Coord2, DrawObjectState, 1024> DrawDataType;
    typedef sdb::L3DataIterator<sdb::Coord2, DrawObjectState, 1024> DrawIteratorType;
    
public:
    DrawableScene();
    ~DrawableScene();

    void setContext(QOpenGLContext *x);

/// within gl context
    void cleanup();
    void initializeScene();
    void draw(const QMatrix4x4 &proj, const QMatrix4x4 &cam);

    void enqueueCreateDrawable(DrawableObject *d, int groupId);
    void enqueueEditDrawable(int objectId, int groupId);
    void enqueueHideDrawable(int objectId, int groupId);
    void enqueueRemoveDrawable(int objectId, int groupId);
/// remove entire group
    void enqueueRemoveDrawable(int groupId);

    QOpenGLContext *context();
    
    void lock();
    void unlock();

private:

};

}

#endif
