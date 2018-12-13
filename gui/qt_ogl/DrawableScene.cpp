#include "DrawableScene.h"
#include "WireframeProgram.h"
#include "SolidProgram.h"
#include "DrawableObject.h"
#include <QDebug>

namespace alo {

DrawableScene::DrawableScene() :
m_frameNumber(0),
m_programId(0)
{
    m_program[0] = nullptr;
    m_program[1] = nullptr;
}

DrawableScene::~DrawableScene()
{}

void DrawableScene::setContext(QOpenGLContext *x)
{ m_ctx = x; }

QOpenGLContext *DrawableScene::context()
{ return m_ctx; }

void DrawableScene::cleanup()
{  
	delete m_program[0];
    m_program[0] = nullptr;
    delete m_program[1];
    m_program[1] = nullptr;

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
    if(!m_program[0]) {
        m_program[0] = new WireframeProgram;
        m_program[0]->initializeProgram(m_ctx);
    }
    if(!m_program[1]) {
        m_program[1] = new SolidProgram;
        m_program[1]->initializeProgram(m_ctx);
    }
}

void DrawableScene::draw(const QMatrix4x4 &proj, const QMatrix4x4 &cam)
{
    lock();
    BaseProgram *program = m_program[m_programId];
    program->beginProgram(proj);

    int destoyedCount = 0;

    DrawDataType *block = m_drawQueue.begin();
    while(block) {
        for (int i=0;i<block->count();++i) { 
            DrawObjectState &it = block->value(i);

            if(it._state <= stDestroyed) {
                destoyedCount++;
                continue;
            }

            if(it._state == stCanDestroy) {
                delete it._object;
                it._state = stDestroyed;
                continue;
            }

            if(it._state <= stWaitDestroy) {
                it._state--;
                continue;
            }

            if(it._state == stHidden)
                continue;

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
            program->setModelView(world, modelView);
            program->setWireColor(DarkColors[d->drawId() & 15]);
            d->draw(m_ctx);
        }
        block = m_drawQueue.next(block);
    } 
    
    program->endProgram();
    m_frameNumber++;
    if(destoyedCount > 31) compressQueue();
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

void DrawableScene::compressQueue()
{
    DrawDataType *block = m_drawQueue.begin();
    while(block) {
        for (int i=0;i<block->count();++i) { 
            if(block->isSingular()) break;

            DrawObjectState &it = block->value(i);

            if(it._state <= stDestroyed) {
                block->remove(block->key(i));
                i--;
            }
        }
        block = m_drawQueue.next(block);
    } 
}

void DrawableScene::selectProgram(ShaderProgramId x)
{ m_programId = x; }

}
