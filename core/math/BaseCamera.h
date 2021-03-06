/*
 *  BaseCamera.h
 *  aloe
 *
 *
 */

#ifndef ALO_MATH_BASE_CAMERA_H
#define ALO_MATH_BASE_CAMERA_H

#include "Matrix44F.h"
#include "Float4.h"

namespace alo {

class BaseCamera {

/// eye to center of interest
    float m_focusDistance;
    float m_nearClipPlane, m_farClipPlane;
    float fHorizontalAperture;
    unsigned fPortWidth, fPortHeight;
	Matrix44F fSpace, fInverseSpace;
    
public:

	enum { ActFocusIn3D = 1178 };

	BaseCamera();
	virtual ~BaseCamera();

	void lookAt(const Float4 &p);
	
	virtual bool isOrthographic() const;
	void reset(const Vector3F & pos);
	void lookFromTo(Vector3F & from, Vector3F & to);
	void setFocusDistance(const float &x);
	void setPortWidth(unsigned w);
	void setPortHeight(unsigned h);
	void setHorizontalAperture(float w);
	void updateInverseSpace();
	void getMatrix(float* m) const;
	void tumble(int x, int y);
	void track(int x, int y);
	virtual void zoom(int y);
	void moveForward(int y);
	
	char screenToWorldPoint(int x, int y, Vector3F & worldPos) const;
	void screenToWorldVector(int x, int y, Vector3F & worldVec) const;
	virtual void screenToWorldVectorAt(int x, int y, float depth, Vector3F & worldVec) const;
/// origin on near clipping plane
/// (x,y) is pixel location (0,0) is top-left 
	virtual void incidentRay(int x, int y, Vector3F & origin, Vector3F & worldVec) const;
/// world position of space
	Vector3F eyePosition() const;
/// world direction +z of space pointing out of screen
	Vector3F eyeDirection() const;
/// center of focal plane at near clipping depth
	Vector3F frameCenter() const;
	
/// port width / height
	float aspectRatio() const;
	float nearClipPlane() const;
	float farClipPlane() const;
	int portWidth() const;
	int portHeight() const;
	float heightWidthRatio() const;
    const float &focusDistance() const;
	
	virtual float fieldOfView() const;
	virtual float frameWidth() const;
	virtual float frameHeight() const;
	virtual float frameWidthRel() const;
	float getHorizontalAperture() const;
	virtual void frameCorners(Vector3F & bottomLeft, Vector3F & bottomRight, Vector3F & topRight, Vector3F & topLeft) const;
	void copyTransformFrom(BaseCamera * another);
/// pixel origin is left-top
/// right-top is 0.5,0.5
/// left-bottom is -0.5,-0.5
	void getScreenCoord(float& cx, float& cy,
			const int& px, const int& py) const;
	
	void setNearClipPlane(float x);
	void setFarClipPlane(float x);
	virtual void setFieldOfView(float x);
	
	void traverse(const Vector3F & v);
	void setViewTransform(const Matrix44F & mat,
				const float & focusDistance);
	
    Vector3F transformToWorld(const Vector3F &x) const;
    Vector3F transformNormalToWorld(const Vector3F &x) const;
    const Matrix44F &space() const;
    const Matrix44F &inverseSpace() const;

protected:

	virtual void calculateFocusDistanceAperture(const float &radius);

};

}

#endif
