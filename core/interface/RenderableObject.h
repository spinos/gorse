/*
 *  RenderableObject.h
 *  vachellia
 *
 */

#ifndef VCHL_RENDERABLE_OBJECT_H
#define VCHL_RENDERABLE_OBJECT_H

namespace alo {

class Ray;
class IntersectResult;

class RenderableObject {

	int m_drawId;

public:
	RenderableObject();
	virtual ~RenderableObject();

	void setDrawId(int x);
    const int& drawId() const;

	virtual bool intersectRay(const Ray& aray, IntersectResult& result);

};

}

#endif
