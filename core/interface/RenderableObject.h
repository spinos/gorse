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
	void setVisible(bool x);
    void setOverlay(bool x);
    
    const int& objectId() const;
    bool isToDestroy() const;
    bool isHidden() const;
    bool isOverlay() const;
    
	virtual bool intersectRay(IntersectResult& result) const = 0;

    const float *c_aabb() const;
    void getAabb(float *y) const;
    
protected:
    float *aabb();
    void extractAabb(float *y) const;
    void resetAabb();
    void setAabb(float x0, float y0, float z0,
                float x1, float y1, float z1);
    void setAabbNull();

private:
    enum RenderableState {
        stUnknown = 0,
        stWaitDestroy,
        stNormal = 3,
        stOverlay = 128,
        stOverlayMask = 255,
        stHiddenMask = 256
    };
	int m_state;
	
};

}

#endif
