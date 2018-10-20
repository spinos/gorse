#include "DrawableScene.h"

#include <qt_ogl/WireframeProgram.h>
#include <qt_ogl/DrawableObject.h>

namespace alo {

DrawableScene::DrawableScene()
{
	m_program1 = nullptr;
}

DrawableScene::~DrawableScene()
{}

void DrawableScene::cleanup()
{  
	delete m_program1;
    m_program1 = nullptr;

    for (auto i : m_drawables)
    	delete i;
}

void DrawableScene::initializeScene()
{
	m_program1 = new WireframeProgram();
    m_program1->initializeProgram();
}

void DrawableScene::draw(const QMatrix4x4 &proj, const QMatrix4x4 &cam)
{

    m_program1->beginProgram(proj);

	for (DrawableObject *i : m_drawables) {
		const QMatrix4x4 &world = i->worldMatrix();
    	const QMatrix4x4 modelView = cam * world;
    	m_program1->setModelView(world, modelView);
        i->draw();
	}

    m_program1->endProgram();
}

DrawableObject *DrawableScene::createDrawable()
{
	DrawableObject *o = new DrawableObject;
	m_drawables.push_back(o);
	return o;
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

}
