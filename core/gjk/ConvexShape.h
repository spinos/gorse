/*
 *  ConvexShape.h
 *  
 *
 *  Created by jian zhang on 11/4/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "GjkIntersection.h"
#include <math/Float2.h>
#include <math/Matrix44F.h>
#include <math/BarycentricCoordinate.h>

namespace alo {
    
namespace cvx {
    
    enum ShapeType {
        TUnknown = 0,
        TSphere = 1,
        TCube = 2,
        TCapsule = 3,
		TTriangle = 4,
		TBox = 5,
		TTetrahedron = 6,
		THexagon = 7,
		TQuad = 8
    };

class Frustum {

	/// 1-------0
	/// |       |
	/// |  far  |
	/// 2-------3
	///
	/// 5-------4
	/// |       |
	/// |  near |
	/// 6-------7
	///
	Vector3F m_corners[8];
	
public:
	Frustum();
	
	void set(float nearClip, float farClip,
			float horizontalAperture, float verticalAperture,
			float angleOfView,
			const Matrix44F & space);
			
	Vector3F * x();
	Vector3F X(int idx) const;
	int numPoints() const;
	Vector3F supportPoint(const Vector3F & v, Vector3F * localP = NULL) const;

	/// 2----p1 p1---3 
	/// |     | |    |
	/// |     | |    |
	/// 0----p0 p0---1
	///
	/// 6----p3 p3---7
	/// |     | |    |
	/// |     | |    |
	/// 4----p2 p2---5
	///
	/// 2------------3 
	/// |            |
	/// p0-----------p1
	/// p0-----------p1 
	/// |            |
	/// 0------------1
	///
	/// 6------------7
	/// |            |
	/// p2-----------p3
	/// p2-----------p3
	/// |            |
	/// 4------------5
	///
	void split(Frustum & child0, Frustum & child1, float alpha, bool alongX = true) const;
	bool intersectBBox(const BoundingBox & b) const;
	
protected:

private:

};

class Sphere {
  
    Vector3F m_p;
    float m_r;
    
public:
    Sphere();
    void set(const Vector3F & x, const float & r);
    
    BoundingBox calculateBBox() const;
    
	float distanceTo(const Vector3F & p) const;
	
	template<typename T>
	bool intersect(const T * b) const
	{
		BoundingBox bb = b->calculateBBox();
		BoundingBox ba = calculateBBox();
		if(!ba.intersect(bb) ) {
			return false;
		}
			
		return gjk::Intersect1<T, Sphere>::Evaluate(*b, *this); 
	}
	
	template<typename T>
	bool narrowphaseIntersect(const T * b, const float & shellThickness) const
	{
		if(!gjk::Intersect1<T, Sphere>::Evaluate(*b, *this) )
			return false;
			
		const int n = b->numPoints();
		for(int i=0; i<n; ++i) {
			if(b->X(i).distanceTo(m_p) > m_r)
				return true;
		}
/// all inside
		return false;
	}
	
	Vector3F supportPoint(const Vector3F & v, Vector3F * localP = 0) const;
	
	bool intersectBBox(const BoundingBox & b) const;
	bool intersectPoint(const Vector3F & p) const;
	float rayIntersect(const Ray & r) const;
	const Vector3F & center() const;
	const float & radius() const;
	
};

class Cube {
    
    Vector3F m_p;
    float m_r;
    
public:
    Cube();
    void set(const Vector3F & x, const float & r);
    
    BoundingBox calculateBBox() const;
	bool intersect(const Ray &ray, float *hitt0, float *hitt1) const;
	Vector3F calculateNormal() const;
	
	template<typename T>
	bool exactIntersect(const T & b) const {
		return true;
	}
    
    static ShapeType ShapeTypeId;
	static std::string GetTypeStr();
	
	virtual float distanceTo(const Vector3F & p) const;
	
	template<typename T>
	bool intersect(const T * b) const
	{ 
		std::cout<<" todo cube intersect";
		return false; 
	}
	
};

class Box {
    
    Vector3F m_low; int m_pad0;
	Vector3F m_high; int m_pad1;
    
public:
    Box();
    void set(const Vector3F & lo, const Vector3F & hi);
	void set(const float * m);
	void expand(const float & d);
    
    BoundingBox calculateBBox() const;
	bool intersect(const Ray &ray, float *hitt0, float *hitt1) const;
	Vector3F calculateNormal() const;
	
	template<typename T>
	bool exactIntersect(const T & b) const {
		return true;
	}
    
    static std::string GetTypeStr();
	
	virtual float distanceTo(const Vector3F & p) const;
	
	template<typename T>
	bool intersect(const T * b) const
	{ 
		return gjk::Intersect1<T, Box>::Evaluate(*b, *this); 
	}
	
	Vector3F X(int i) const;
	Vector3F supportPoint(const Vector3F & v, Vector3F * localP = 0) const;
	
	bool intersectBBox(const BoundingBox & b) const;
	
	float rayIntersect(const Ray & r) const;
	
};

class Capsule {
    
    Vector3F m_p0;
    float m_r0;
    Vector3F m_p1;
    float m_r1;
    
public:
    Capsule();
    void set(const Vector3F & x0, const float & r0,
            const Vector3F & x1, const float & r1);
    
    BoundingBox calculateBBox() const;
    
    static ShapeType ShapeTypeId;
};

class Triangle {

	Vector3F m_p0; int m_nc0;
	Vector3F m_p1; int m_nc1;
	Vector3F m_p2; int m_nc2;
	Float2 m_uv[3];
	
public:
	Triangle();
	void set(const Vector3F & p0, const Vector3F & p1,
			const Vector3F & p2);
	void setP(const Vector3F & p, const int & idx);
	void resetNC();
	void setN(const Vector3F & n, const int & idx);
	void setC(const Vector3F & c, const int & idx);
	void setInd(const int & x, const int & idx);
	
	const Vector3F * p(int idx) const;
	const Vector3F & P(int idx) const;
	Vector3F N(int idx) const;
	Vector3F C(int idx) const;
	const int & ind0() const;
	const int & ind1() const;
	Vector3F center() const;
	Vector3F calculateNormal() const;
	float calculateArea() const;
	void translate(const Vector3F & v);

	BoundingBox calculateBBox() const;
	bool rayIntersect(const Ray &ray, float *hitt0, float *hitt1) const;
	void getBoundingSphere(Sphere & s) const;
	
	template<typename T>
	bool exactIntersect(const T & b) const;
		
	template<typename T>
	void closestToPoint(T * result) const;
	
	bool sampleP(Vector3F & dst, const BoundingBox &  b) const;
	
	void setUVs(const Float2 * src);
	Float2 interpolateTexcoord(const float * contribs) const;
	Vector3F interpolateColor(const float * contribs) const;
					
	const Float2 & uv(const int & i) const;
	
	const Vector3F & X(int idx) const;
	const Vector3F & supportPoint(const Vector3F & v, Vector3F * localP = NULL) const;
	
	static ShapeType ShapeTypeId;
	static std::string GetTypeStr();
	
private:
	
};

template<typename T>
bool Triangle::exactIntersect(const T & b) const 
{
	return gjk::Intersect1<Triangle, T>::Evaluate(*this, b);
}

template<typename T>
void Triangle::closestToPoint(T * result) const
{
	BarycentricCoordinate & bar = result->_bar;
	bar.create(m_p0, m_p1, m_p2);
	float d = bar.project(result->_toPoint);
	if(d >= result->_distance) {
		return;
	}
	bar.compute();
	if(!bar.insideTriangle()) {
		bar.computeClosest();
	}
	
	const Vector3F clampledP = bar.getClosest();
	d = clampledP.distanceTo(result->_toPoint );
	if(d >= result->_distance) {
		return;
	}
	
	result->_distance = d;
	result->_hasResult = true;
	result->_hitPoint = clampledP;
	result->_hitNormal = bar.getNormal();
	if(result->_isFast ) {
		return;
	}
	
	result->_contributes[0] = bar.getV(0);
	result->_contributes[1] = bar.getV(1);
	result->_contributes[2] = bar.getV(2);
	result->_igeometry = ind0();
	result->_icomponent = ind1();
	result->_isource = result->_currentSourceInd;
}

/// 3 -- 2
/// |    |
/// 0 -- 1
class Quad {
	
	Vector3F m_p[4];
	
public:
	Quad();
	void set(const Vector3F & p0, const Vector3F & p1,
			const Vector3F & p2, const Vector3F & p3);
	const Vector3F * p(int idx) const;
	const Vector3F & P(int idx) const;
	Vector3F calculateNormal() const;
	
	static ShapeType ShapeTypeId;
	static std::string GetTypeStr();
	
};

class Tetrahedron {

	Vector3F m_p[4];
	
public:
	Tetrahedron();
	void set(const Vector3F & p0, const Vector3F & p1,
			const Vector3F & p2, const Vector3F & p3);
	
	BoundingBox calculateBBox() const;
	
	int numPoints() const;
	const Vector3F & X(int idx) const;
	Vector3F supportPoint(const Vector3F & v, Vector3F * localP = NULL) const;
	
	Vector3F getCenter() const;
/// 0-----------3
/// | \        /
/// |   \     /
/// |     \  /
/// 1-------2
/// 0: 0 1 2 
/// 1: 0 2 3
/// 2: 0 3 1
/// 3: 1 3 2  
	Vector3F getFaceCenter(const int & i) const;
/// 0: 0 1
/// 1: 0 2
/// 2: 0 3
/// 3: 1 2
/// 4: 1 3
/// 5: 2 3
	Vector3F getEdgeCenter(const int & i) const;
	void getFace(Triangle & tri, const int & i) const;

    bool isPointInside(const Vector3F & q) const;
    void circumSphere(Vector3F & center, float & radius) const;
	void getCenterRadius(Vector3F & center, float & radius) const;
	const Vector3F & P(int idx) const;
	
	static ShapeType ShapeTypeId;
	static std::string GetTypeStr();
	
private:

};

template<int N>
class Hull {
	
	BoundingBox m_bbox;
	Float4 m_plane[N];
	
public:
	Hull();
	
	int numPlanes() const;
	Float4 * plane(int x);
	const Float4 & Plane(int x) const;
	
	BoundingBox * bbox();
	const BoundingBox & BBox() const;
	
};

template<int N>
Hull<N>::Hull()
{}

template<int N>
int Hull<N>::numPlanes() const
{ return N; }

template<int N>
Float4 * Hull<N>::plane(int x)
{ return &m_plane[x]; }

template<int N>
const Float4 & Hull<N>::Plane(int x) const
{ return m_plane[x]; }

template<int N>
BoundingBox * Hull<N>::bbox()
{ return &m_bbox; }

template<int N>
const BoundingBox & Hull<N>::BBox() const
{ return m_bbox; }

typedef Hull<5> PyramidHull;
typedef Hull<6> FrustumHull;

}

}
