#include "DrawableScene.h"
#include <qt_ogl/WireframeProgram.h>
#include <qt_ogl/DrawableObject.h>
#include <QDebug>

namespace alo {

DrawableScene::DrawableScene() :
m_program1(nullptr)
{}

DrawableScene::~DrawableScene()
{}

void DrawableScene::setContext(QOpenGLContext *x)
{ m_ctx = x; }

QOpenGLContext *DrawableScene::context()
{ return m_ctx; }

void DrawableScene::cleanup()
{  
	delete m_program1;
    m_program1 = nullptr;

    DrawDataType *block = m_drawQueue.begin();
    while(block) {
        for (int i=0;i<block->count();++i) { 
            DrawObjectState &it = block->value(i);

            if(it._state >= stWaitDestroy) delete it._object;
        }
        block = m_drawQueue.next(block);
    }
}

void DrawableScene::initializeScene()
{
    if(!m_program1) {
        m_program1 = new WireframeProgram;
        m_program1->initializeProgram(m_ctx);
    }
}

void DrawableScene::draw(const QMatrix4x4 &proj, const QMatrix4x4 &cam)
{
    m_program1->beginProgram(proj);

    lock();
    DrawDataType *block = m_drawQueue.begin();
    while(block) {
        for (int i=0;i<block->count();++i) { 
            DrawObjectState &it = block->value(i);

            if(it._state <= stDestroyed || it._state == stHidden)
                continue;

            if(it._state == stWaitDestroy) {
                delete it._object;
                it._state = stDestroyed;
                continue;
            }

            if(it._state == stWaitEdit) {
                it._object->update(m_ctx);
                it._state = stNormal;
            }

            if(it._state == stWaitCreate) {
                it._object->create(m_ctx);
                it._state = stNormal;
            }

            DrawableObject *d = it._object;
            const QMatrix4x4 &world = d->worldMatrix();
            const QMatrix4x4 modelView = cam * world;
            m_program1->setModelView(world, modelView);
            d->draw(m_ctx);
        }
        block = m_drawQueue.next(block);
    }
    unlock();

    m_program1->endProgram();
}

void DrawableScene::enqueueCreateDrawable(DrawableObject *d, int groupId)
{ 
    DrawObjectState a;
    a._state = stWaitCreate;
    a._object = d;
    m_drawQueue.insert(sdb::Coord2(d->drawId(), groupId), a);
}

void DrawableScene::enqueueEditDrawable(int objectId, int groupId)
{ 
    DrawObjectState *a = m_drawQueue.find(sdb::Coord2(objectId, groupId) );
    if(a) a->_state = stWaitEdit;
}

void DrawableScene::enqueueRemoveDrawable(int objectId, int groupId)
{ 
    DrawObjectState *a = m_drawQueue.find(sdb::Coord2(objectId, groupId) );
    if(a) a->_state = stWaitDestroy;
}

void DrawableScene::enqueueHideDrawable(int objectId, int groupId)
{
    DrawObjectState *a = m_drawQueue.find(sdb::Coord2(objectId, groupId) );
    if(a) a->_state = stHidden;
}

void DrawableScene::enqueueRemoveDrawable(int groupId)
{
    DrawIteratorType it = m_drawQueue.begin(sdb::Coord2(-1,groupId));
    for(;!it.done();it.next()) {
        if(it.first.y > groupId) break;
        if(it.first.y < groupId) continue;
        if(it.second->_state > stWaitDestroy) it.second->_state = stWaitDestroy;
    }
}

void DrawableScene::lock()
{ m_mutex.lock(); }

void DrawableScene::unlock()
{ m_mutex.unlock(); }

}
