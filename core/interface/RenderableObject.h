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

	int m_objectId;

public:

	enum RenderableState {
        stUnknown = 0,
        stWaitDestroy,
        stHidden,
        stNormal,
        stOverlay
    };

	RenderableObject();
	virtual ~RenderableObject();

	void setObjectId(int x);
    const int& objectId() const;

	virtual bool intersectRay(const Ray& aray, IntersectResult& result);

protected:

private:

	RenderableState m_state;
	
};

}

#endif
