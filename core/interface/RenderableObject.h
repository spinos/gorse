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

    float m_aabb[6];
	int m_objectId;

public:
	RenderableObject();
	virtual ~RenderableObject();

	void setObjectId(int x);
	void setToDestroy();
	void setVisibility(bool x);
    
    const int& objectId() const;
    bool isToDestroy() const;
    bool isHidden() const;
    bool isOverlay() const;
    
	virtual bool intersectRay(const Ray& aray, IntersectResult& result);

protected:
    float *aabb();

private:
    enum RenderableState {
        stUnknown = 0,
        stWaitDestroy,
        stNormal = 3,
        stOverlay = 7,
        stHiddenMask = 256
    };
	int m_state;
	
};

}

#endif
