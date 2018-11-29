#include "DrawableScene.h"
#include <qt_ogl/WireframeProgram.h>
#include <qt_ogl/DrawableObject.h>
#include <QDebug>

namespace alo {

DrawableScene::DrawableScene()
{
	m_program1 = nullptr;
}

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

    for (auto i : m_drawables)
    	delete i;
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
    while(m_createQueue.size() > 0) {
        DrawableObject *i = m_createQueue.front();
        i->create(m_ctx);
        m_drawables.push_back(i);
        m_createQueue.erase(m_createQueue.begin());
    }

    while(m_editQueue.size() > 0) {
        DrawableObject *i = m_editQueue.front();
        i->update(m_ctx);
        m_editQueue.erase(m_editQueue.begin());
    }

    while(m_removeQueue.size() > 0) {
        DrawableObject *i = m_removeQueue.front();
        removeDrawable(i);
        m_removeQueue.erase(m_removeQueue.begin());
    }
    unlock();

    m_program1->beginProgram(proj);

    std::vector<DrawableObject *>::iterator it = m_drawables.begin();
	for (;it!=m_drawables.end();++it) { 
		const QMatrix4x4 &world = (*it)->worldMatrix();
    	const QMatrix4x4 modelView = cam * world;
    	m_program1->setModelView(world, modelView);
        (*it)->draw(m_ctx);
	}

    m_program1->endProgram();
}

void DrawableScene::enqueueCreateDrawable(DrawableObject *d, int groupId)
{ 
    m_createQueue.push_back(d); 
}

void DrawableScene::enqueueEditDrawable(DrawableObject *d)
{ 
    m_editQueue.push_back(d); 
}

void DrawableScene::enqueueRemoveDrawable(DrawableObject *d)
{ 
    m_removeQueue.push_back(d);
}

bool DrawableScene::removeDrawable(DrawableObject *k)
{
	std::vector<DrawableObject *>::iterator it = m_drawables.begin();
	for (;it != m_drawables.end();++it) {
        if(*it == k) {
        	delete *it;
        	m_drawables.erase(it);
        	return true;
        }
	}
    return false;
}

void DrawableScene::lock()
{ m_mutex.lock(); }

void DrawableScene::unlock()
{ m_mutex.unlock(); }

}
