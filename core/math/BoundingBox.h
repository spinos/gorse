/*
 *  BoundingBox.h
 *  aloe
 *
 *  Created by jian zhang on 10/17/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef ALO_BOUNDING_BOX_H
#define ALO_BOUNDING_BOX_H

#include "Vector3F.h"

namespace alo {

class Ray;

class BoundingBox {

public:

	BoundingBox();
	BoundingBox(const float & x0, const float & y0, const float & z0,
	            const float & x1, const float & y1, const float & z1);
	BoundingBox(const float * d);
	
	void setCenterHalfSpan(const float* & c);
	void set(const Vector3F & center,
			const float & h);
	void reset();
	void setOne();
	void setMin(float x, int axis);
	void setMax(float x, int axis);
	void setMin(float x, float y, float z);
	void setMax(float x, float y, float z);
	void update(const Vector3F & p);
	void updateMin(const Vector3F & p);
	void updateMax(const Vector3F & p);
	
	const int getLongestAxis() const;
	const float getLongestDistance() const;
	const float getMin(int axis) const;
	const float getMax(int axis) const;
	const Vector3F getMin() const;
	const Vector3F getMax() const;
	const float area() const;
	const float volume() const;
	const float crossSectionArea(const int &axis) const;
	const float distance(const int &axis) const;
	const Vector3F normal(const int & i) const;
	
	void split(int axis, float pos, BoundingBox & lft, BoundingBox & rgt) const;
	void splitLeft(int axis, float pos, BoundingBox & lft) const;
	void splitRight(int axis, float pos, BoundingBox & rgt) const;
	void expandBy(const BoundingBox &another);
	void expandBy(const Vector3F & pos);
	void expandBy(const Vector3F & pos, float r);
	void shrinkBy(const BoundingBox &another);
	void expand(float v);
	Vector3F center() const;
	Vector3F lowCorner() const;
	Vector3F highCorner() const;
	const Vector3F corner(const int & i) const;
	char touch(const BoundingBox & b) const;
	
	float distanceTo(const Vector3F & pnt) const;
    float radius() const;
	float radiusXZ() const;
	
	bool intersect(const BoundingBox & another) const;
	bool intersect(const BoundingBox & another, BoundingBox * tightBox) const;
	char intersect(const Ray &ray, float *hitt0=NULL, float *hitt1=NULL) const;
	
    char isPointInside(const Vector3F & p) const;
	char isPointAround(const Vector3F & p, float threshold) const;
	char isBoxAround(const BoundingBox & b, float threshold) const;
	bool isBoxOnBoundary(const BoundingBox & b) const;
	char inside(const BoundingBox & b) const;
	int pointOnSide(const Vector3F & v) const;
	int pointOnEdge(const Vector3F & v) const;
	char isValid() const;
	void round();
	friend std::ostream& operator<<(std::ostream &output, const BoundingBox & p);
	
	const BoundingBox & calculateBBox() const;
    int numPoints() const;
    Vector3F supportPoint(const Vector3F & v, Vector3F * localP = NULL) const;
    Vector3F X(int idx) const;
	const float * data() const;
	
	void translate(const Vector3F & ta);
	void translate(const float * t);
	void scale(const float & sc);
	void scale(const float * sc);
	int numFlatAxis(const float & threshold) const;
/// longest first
/// shortest last
	void getSizeOrder(int * d) const;
	void putInside(Vector3F & p) const;
	void getSubBox(BoundingBox & dst,
				const Vector3F & coord,
				const float & delta) const;
	
	float m_data[6];
	int m_padding0, m_padding1;
};
}
#endif