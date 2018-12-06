#include "DrawableScene.h"
#include <qt_ogl/WireframeProgram.h>
#include <qt_ogl/DrawableObject.h>
#include <QDebug>

namespace alo {

DrawableScene::DrawableScene() :
m_program1(nullptr),
m_frameNumber(0)
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
    lock();
    m_program1->beginProgram(proj);

    DrawDataType *block = m_drawQueue.begin();
    while(block) {
        for (int i=0;i<block->count();++i) { 
            DrawObjectState &it = block->value(i);

            if(it._state <= stDestroyed || it._state == stHidden)
                continue;

            if(it._state == stCanDestroy) {
                delete it._object;
                it._state = stDestroyed;
                continue;
            }

            if(it._state <= stWaitDestroy) {
                it._state--;
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
            m_program1->setWireColor(DarkColors[d->drawId() & 15]);
            d->draw(m_ctx);
        }
        block = m_drawQueue.next(block);
    } 
    
    m_program1->endProgram();
    m_frameNumber++;
    unlock();
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

void DrawableScene::enqueueShowDrawable(int objectId, int groupId)
{
    DrawObjectState *a = m_drawQueue.find(sdb::Coord2(objectId, groupId) );
    if(a) a->_state = stNormal;
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

const int &DrawableScene::frameNumber() const
{ return m_frameNumber; }

}
