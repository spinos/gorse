/*
 *  DrawableScene.h
 *  aloe
 *
 *  draw queue 
 *  index (object_id, group_id)
 *  value object
 *  create, edit, destroy
 *  wait for 3 frames allowing drawable 
 *  to finish unsynchronized buffer before dstruction
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

class BaseProgram;
class DrawableObject;

class DrawableScene
{
	BaseProgram *m_program[2];
    QOpenGLContext *m_ctx;
    QMutex m_mutex;

    enum ObjectState {
        stUnknown = 0,
        stDestroyed,
        stCanDestroy,
        stWaitDestroy=5,
        stHidden,
        stWaitEdit,
        stWaitCreate,
        stNormal
    };

    struct DrawObjectState {
        int _state;
        DrawableObject *_object;
    };

    sdb::L3Tree<sdb::Coord2, DrawObjectState, 2048, 512, 1024 > m_drawQueue;
    typedef sdb::L3Node<sdb::Coord2, DrawObjectState, 1024> DrawDataType;
    typedef sdb::L3DataIterator<sdb::Coord2, DrawObjectState, 1024> DrawIteratorType;
    int m_frameNumber;
    int m_programId;
    
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
    void enqueueShowDrawable(int objectId, int groupId);
    void enqueueRemoveDrawable(int objectId, int groupId);
/// remove entire group
    void enqueueRemoveDrawable(int groupId);

    QOpenGLContext *context();
    
    void lock();
    void unlock();

    const int &frameNumber() const;

    enum ShaderProgramId {
        WireShaderProgramId = 0,
        SolidShaderProgramId
    };

    void selectProgram(ShaderProgramId x);

private:
/// remove destroyed ones
    void compressQueue();

};

}

#endif
