#ifndef DRAWABLE_SCENE_H
#define DRAWABLE_SCENE_H

#include <QMatrix4x4>
#include <vector>

namespace alo {

class WireframeProgram;
class DrawableObject;

class DrawableScene
{
	WireframeProgram *m_program1;
    std::vector<DrawableObject *> m_drawables;

public:
    DrawableScene();
    ~DrawableScene();

    void cleanup();

    void initializeScene();
    void draw(const QMatrix4x4 &proj, const QMatrix4x4 &cam);

    DrawableObject *createDrawable();
    bool removeDrawable(DrawableObject *k);

private:

};

}

#endif
