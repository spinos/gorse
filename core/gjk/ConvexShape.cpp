/*
 *  Frustum.cpp
 *  
 *
 *  Created by jian zhang on 11/4/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include "ConvexShape.h"
#include <math/Quantization.h>
#include <math/line_math.h>
#include <math/tetrahedron_math.h>
#include <math/Ray.h>
#include <math/miscfuncs.h>

namespace alo {
    
namespace cvx {

Frustum::Frustum() {}

void Frustum::set(float nearClip, float farClip,
			float horizontalAperture, float verticalAperture,
			float angleOfView,
			const Matrix44F & space)
{
	const float frm = tan(angleOfView/360.f * 3.1415927f); // half angle
	float h_fov = horizontalAperture * frm;
    float v_fov = verticalAperture * frm;

    float fright = farClip * h_fov;
    float ftop = farClip * v_fov;

    float nright = nearClip * h_fov;
    float ntop = nearClip * v_fov;
	
	m_corners[0].set(-fright, -ftop, -farClip);
	m_corners[1].set( fright, -ftop, -farClip);
	m_corners[2].set(-fright,  ftop, -farClip);
	m_corners[3].set( fright,  ftop, -farClip);
	m_corners[4].set(-nright, -ntop, -nearClip);
	m_corners[5].set( nright, -ntop, -nearClip);
	m_corners[6].set(-nright,  ntop, -nearClip);
	m_corners[7].set( nright,  ntop, -nearClip);
	
	int i = 0;
    for(; i<8; i++) m_corners[i] = space.transform(m_corners[i]);
}

Vector3F * Frustum::x()
{ return m_corners; }

Vector3F Frustum::X(int idx) const
{ return m_corners[idx]; }

int Frustum::numPoints() const
{ return 8; }

Vector3F Frustum::supportPoint(const Vector3F & v, Vector3F * localP) const
{ 
    float maxdotv = -1e18f;
    float dotv;
	
    Vector3F res, q;
    for(int i=0; i < numPoints(); i++) {
        q = m_corners[i];
        dotv = q.dot(v);
        if(dotv > maxdotv) {
            maxdotv = dotv;
            res = q;
            if(localP) *localP = q;
        }
    }
    
    return res;
}

void Frustum::split(Frustum & child0, Frustum & child1, float alpha, bool alongX) const
{
	const float oneMAlpha = 1.f - alpha;
	child0 = *this;
	child1 = *this;
	Vector3F * lft = child0.x();
	Vector3F * rgt = child1.x();
	Vector3F p0, p1, p2, p3;
	if(alongX) {
		p0 = X(0) * oneMAlpha + X(1) * alpha;
		p1 = X(2) * oneMAlpha + X(3) * alpha;
		p2 = X(4) * oneMAlpha + X(5) * alpha;
		p3 = X(6) * oneMAlpha + X(7) * alpha;
		lft[1] = p0;
		rgt[0] = p0;
		lft[3] = p1;
		rgt[2] = p1;
		lft[5] = p2;
		rgt[4] = p2;
		lft[7] = p3;
		rgt[6] = p3;
	}
	else {
		p0 = X(0) * oneMAlpha + X(2) * alpha;
		p1 = X(1) * oneMAlpha + X(3) * alpha;
		p2 = X(4) * oneMAlpha + X(6) * alpha;
		p3 = X(5) * oneMAlpha + X(7) * alpha;
		lft[2] = p0;
		rgt[0] = p0;
		lft[3] = p1;
		rgt[1] = p1;
		lft[6] = p2;
		rgt[4] = p2;
		lft[7] = p3;
		rgt[5] = p3;
	}
}

bool Frustum::intersectBBox(const BoundingBox & b) const
{ return gjk::Intersect1<BoundingBox, Frustum>::Evaluate(b, *this); }

Sphere::Sphere() {}

void Sphere::set(const Vector3F & x, const float & r)
{ m_p = x; m_r = r; }

BoundingBox Sphere::calculateBBox() const
{ return BoundingBox(m_p.x - m_r, m_p.y - m_r, m_p.z - m_r,
                    m_p.x + m_r, m_p.y + m_r, m_p.z + m_r); }

float Sphere::distanceTo(const Vector3F & p) const
{
/// to front 
	return Absolute<float>(p.distanceTo(m_p) - m_r); 
}

Vector3F Sphere::supportPoint(const Vector3F & v, Vector3F * localP) const
{
	Vector3F res = m_p + v.normal() * m_r;
		
	if(localP) *localP = res;
	return res;
}

bool Sphere::intersectBBox(const BoundingBox & b) const
{ 
	if(b.distanceTo(m_p) > m_r)
		return false; 
	
	for(int i=0; i<8; ++i) {
		if(b.X(i).distanceTo(m_p) > m_r )
			return true;
	}
	
/// all corners inside
	return false;
}

///         t1          tmax
/// a-------e----d------b
///          \   |
///           r  |
///            \ |
///              c 
float Sphere::rayIntersect(const Ray & r) const
{
	const Vector3F b = r.destination();
	float cd = distancePointLine(m_p, r.m_origin, b );
/// no intersect	
	if(cd > m_r)
		return 1e8f;
		
/// proj c on line
	Vector3F pd;
	projectPointLineSegment(pd, cd, m_p, r.m_origin, b );
	
	const Vector3F vac = m_p - r.m_origin;
	const float ac = vac.length();
	
	float ad = vac.dot(r.m_dir); 
	float ae;
	
/// c is behind a
	if(ad <= 0.f) {
		if(ac > m_r)
			return 1e8f;
			
/// on sphere
		else if(ac == m_r)
			return 0.f;
	
/// a inside
/// proj c on line
		float d = sqrt(m_r * m_r - cd * cd);
		Vector3F pe = pd + r.m_dir *d;
		
		ae = (pe - r.m_origin).length();
		if(ae > r.m_tmax )
			return 1e8f;
			
		return ae / r.m_tmax;
	}

/// c is in front of a	
	float ed = 0.f;
	if(ac == m_r)
		return 0.f;
		
	if(ac > m_r) {
		
		if(m_r - cd > 1e-3f)
			ed = sqrt(m_r*m_r - cd*cd);
		
/// first hit t1
		ae = ad - ed;
		if(ae <= 0.f)
			return 0.f;
	}
	else {
		if(m_r - cd > 1e-3f)
			ed = sqrt(m_r*m_r - cd*cd);
			
/// t2
		ae = ad + ed;
	}
	
	if(ae > r.m_tmax)
		return 1e8f;
		
	return ae / r.m_tmax;
}

const Vector3F & Sphere::center() const
{ return m_p; }
	
const float & Sphere::radius() const
{ return m_r; }

bool Sphere::intersectPoint(const Vector3F & p) const
{ return p.distanceTo(m_p) < m_r; }

Cube::Cube() {}

void Cube::set(const Vector3F & x, const float & r)
{ m_p = x; m_r = r; }
 
BoundingBox Cube::calculateBBox() const
{ return BoundingBox(m_p.x - m_r, m_p.y - m_r, m_p.z - m_r,
                    m_p.x + m_r, m_p.y + m_r, m_p.z + m_r); }

bool Cube::intersect(const Ray &ray, float *hitt0, float *hitt1) const
{ return calculateBBox().intersect(ray, hitt0, hitt1); }

Vector3F Cube::calculateNormal() const
{ return Vector3F::XAxis; }

ShapeType Cube::ShapeTypeId = TCube;

std::string Cube::GetTypeStr()
{ return "cube"; }

float Cube::distanceTo(const Vector3F & p) const
{
	BoundingBox bx = calculateBBox();
	float d = bx.distanceTo(p);
	if(d >= 0.f)
		return d;
		
/// inside box
	float dx = m_r - Absolute<float>(p.x - m_p.x);
	float dy = m_r - Absolute<float>(p.y - m_p.y);
	float dz = m_r - Absolute<float>(p.z - m_p.z);
	
	if(dx < dy && dx < dz)
		return -dx;
		
	if(dy < dx && dy < dz)
		return -dy;
		
	return -dz;
}

Box::Box() {}

void Box::set(const Vector3F & lo, const Vector3F & hi)
{ m_low = lo; m_high = hi; }

void Box::set(const float * m)
{
	m_low.set(m[0], m[1], m[2]);
	m_high.set(m[3], m[4], m[5]);
}

void Box::expand(const float & d)
{
	m_low.x -= d; m_low.y -= d; m_low.z -= d;
	m_high.x += d; m_high.y += d; m_high.z += d;
}

BoundingBox Box::calculateBBox() const
{ return BoundingBox(m_low.x, m_low.y, m_low.z,
                    m_high.x, m_high.y, m_high.z); }
					
bool Box::intersect(const Ray &ray, float *hitt0, float *hitt1) const
{ return calculateBBox().intersect(ray, hitt0, hitt1); }

Vector3F Box::calculateNormal() const
{ return Vector3F::XAxis; }

std::string Box::GetTypeStr()
{ return "box"; }

float Box::distanceTo(const Vector3F & p) const
{
	BoundingBox bx = calculateBBox();
	float d = bx.distanceTo(p);
	if(d >= 0.f)
		return d;
		
/// inside box
	Vector3F dp = p - bx.center();
	float dx = bx.distance(0) * .5f - Absolute<float>(dp.x);
	float dy = bx.distance(1) * .5f - Absolute<float>(dp.y);
	float dz = bx.distance(2) * .5f - Absolute<float>(dp.z);
	
	if(dx < dy && dx < dz)
		return -dx;
		
	if(dy < dx && dy < dz)
		return -dy;
		
	return -dz;
}

Vector3F Box::X(int i) const
{
	Vector3F r(m_low);
	if(i<4) { /// -z
		if(i<2) { /// -y
			if(i & 1) /// +x
				r.x = m_high.x;
		}
		else { /// +y
			r.y = m_high.y;
			if(i & 1)
				r.x = m_high.x;
		}
	}
	else { /// +z
		r.z = m_high.z;
		if(i<6) { /// -y
			if(i & 1) /// +x
				r.x = m_high.x;
		}
		else { /// +y
			r.y = m_high.y;
			if(i & 1) /// +x
				r.x = m_high.x;
		}
	}
	return r;
}

Vector3F Box::supportPoint(const Vector3F & v, Vector3F * localP) const
{
	float maxdotv = -1e19f;
    float dotv;
	int ir = 0;
	
    for(int i=0; i < 8; ++i) {
        dotv = X(i).dot(v);
        if(dotv > maxdotv) {
            maxdotv = dotv;
            ir = i;
        }
    }
    if(localP) *localP = X(ir);
    return X(ir);
}

bool Box::intersectBBox(const BoundingBox & b) const
{ return false; }

float Box::rayIntersect(const Ray & r) const
{ return 1e8f; }

Capsule::Capsule() {}

void Capsule::set(const Vector3F & x0, const float & r0,
            const Vector3F & x1, const float & r1)
{
    m_p0 = x0; m_r0 = r0;
    m_p1 = x1; m_r1 = r1;
}
    
BoundingBox Capsule::calculateBBox() const
{
    BoundingBox b;
    b.expandBy(m_p0, m_r0);
    b.expandBy(m_p1, m_r1);
    return b;
}
    
ShapeType Capsule::ShapeTypeId = TCapsule;

Triangle::Triangle()
{}

void Triangle::set(const Vector3F & p0, const Vector3F & p1,
			const Vector3F & p2)
{ m_p0 = p0; m_p1 = p1; m_p2 = p2; }

void Triangle::setP(const Vector3F & p, const int & idx)
{ 
	if(idx == 0) m_p0 = p;
	else if(idx == 1) m_p1 = p;
	else m_p2 = p;
}

void Triangle::resetNC()
{ m_nc0 = m_nc1 = m_nc2 = 0; }

void Triangle::setN(const Vector3F & n, const int & idx)
{
	if(idx == 0) colnor30::encodeN(m_nc0, n);
	else if(idx == 1) colnor30::encodeN(m_nc1, n);
	else  colnor30::encodeN(m_nc2, n);
}

void Triangle::setC(const Vector3F & c, const int & idx)
{
	if(idx == 0) colnor30::encodeC(m_nc0, c);
	else if(idx == 1) colnor30::encodeC(m_nc1, c);
	else  colnor30::encodeC(m_nc2, c);
}

void Triangle::setInd(const int & x, const int & idx)
{ 
	if(idx == 0) m_nc0 = x;
	else if(idx == 1) m_nc1 = x;
	else m_nc2 = x;
}

const Vector3F * Triangle::p(int idx) const
{ 
	if(idx == 0) return &m_p0;
	else if(idx == 1) return &m_p1;
	return &m_p2;
}

const Vector3F & Triangle::P(int idx) const
{ 
	if(idx == 0) return m_p0;
	else if(idx == 1) return m_p1;
	return m_p2;
}

Vector3F Triangle::N(int idx) const
{
	Vector3F r;
	if(idx == 0) colnor30::decodeN(r, m_nc0);
	else if(idx == 1) colnor30::decodeN(r, m_nc1);
	else colnor30::decodeN(r, m_nc2);
	return r;
}

Vector3F Triangle::C(int idx) const
{
	Vector3F r;
	if(idx == 0) colnor30::decodeC(r, m_nc0);
	else if(idx == 1) colnor30::decodeC(r, m_nc1);
	else colnor30::decodeC(r, m_nc2);
	return r;
}

const int & Triangle::ind0() const
{ return m_nc0; }

const int & Triangle::ind1() const
{ return m_nc1; }

Vector3F Triangle::center() const
{ return (m_p0 + m_p1 + m_p2) * .333333f; }

BoundingBox Triangle::calculateBBox() const
{
	BoundingBox b;
    b.expandBy(m_p0);
    b.expandBy(m_p1);
	b.expandBy(m_p2);
    b.expand(b.getLongestDistance() * 1e-4f);
    return b;
}

Vector3F Triangle::calculateNormal() const
{
	Vector3F ab = m_p1 - m_p0;
	Vector3F ac = m_p2 - m_p0;
	Vector3F nor = ab.cross(ac);
	nor.normalize();
	return nor;
}

float Triangle::calculateArea() const
{
	Vector3F d[3]; float l[3];

	d[0] = m_p1 - m_p0;
	l[0] = d[0].length();
	
	d[1] = m_p2 - m_p1;
	l[1] = d[1].length();
	
	d[2] = m_p0 - m_p2;
	l[2] = d[2].length();
	
	return (0.5*sqrt(l[0]*l[0]*l[2]*l[2] - (d[0].dot(d[2]))*(d[0].dot(d[2]))));
}

void Triangle::translate(const Vector3F & v)
{
	m_p0 += v;
	m_p1 += v;
	m_p2 += v;
}

bool Triangle::rayIntersect(const Ray &ray, float *hitt0, float *hitt1) const
{
	const Vector3F nor = calculateNormal();
	
	float ddotn = ray.m_dir.dot(nor);
	
	//if(!ctx->twoSided && ddotn > 0.f) return 0;
	
	float t = (m_p0.dot(nor) - ray.m_origin.dot(nor)) / ddotn;
	
	//std::cout<<"\n Triangle::intersect "<<nor<<" "<<ddotn<<" t "<<t;
	
	if(t < 0.f || t > ray.m_tmax) return 0;
	
	Vector3F onplane = ray.m_origin + ray.m_dir * t;
	Vector3F e01 = m_p1 - m_p0;
	Vector3F x0 = onplane - m_p0;
	if(e01.cross(x0).dot(nor) < 0.f) return 0;
	
	//printf("pass a\n");

	Vector3F e12 = m_p2 - m_p1;
	Vector3F x1 = onplane - m_p1;
	if(e12.cross(x1).dot(nor) < 0.f) return 0;
	
	//printf("pass b\n");
	
	Vector3F e20 = m_p0 - m_p2;
	Vector3F x2 = onplane - m_p2;
	if(e20.cross(x2).dot(nor) < 0.f) return 0;
	
	//printf("pass c\n");
	*hitt0 = t;
	*hitt1 = t;
	
    return true;
}

void Triangle::getBoundingSphere(Sphere & s) const
{ 
	BoundingBox bx = calculateBBox();
	
	s.set(bx.center(), bx.radius() );
}

const Vector3F & Triangle::X(int idx) const
{ return P(idx); }

const Vector3F & Triangle::supportPoint(const Vector3F & v, Vector3F * localP) const
{
	float maxdotv = -1e19f;
    float dotv;
	int ir = 0;
	
    for(int i=0; i < 3; ++i) {
        dotv = P(i).dot(v);
        if(dotv > maxdotv) {
            maxdotv = dotv;
            ir = i;
        }
    }
    if(localP) *localP = P(ir);
    return P(ir);
}

ShapeType Triangle::ShapeTypeId = TTriangle;

std::string Triangle::GetTypeStr()
{ return "triangle"; }

bool Triangle::sampleP(Vector3F & dst, const BoundingBox &  box) const
{
	for(int i=0; i<10; ++i) {
		float a = ((float)(rand() & 1023)) / 1023.f;
		float b = (1.f - a ) * ((float)(rand() & 1023)) / 1023.f;
		float c = 1.f - a - b;
		dst = m_p0 * a + m_p1 * b + m_p2 * c;
		if(box.isPointInside(dst) ) return true;
	}
	return false;
}

void Triangle::setUVs(const Float2 * src)
{ 
	m_uv[0] = src[0];
	m_uv[1] = src[1];
	m_uv[2] = src[2];
}

Float2 Triangle::interpolateTexcoord(const float * contribs) const
{
	return Float2(m_uv[0].x * contribs[0]
			+ m_uv[1].x * contribs[1]
			+ m_uv[2].x * contribs[2],
			m_uv[0].y * contribs[0]
			+ m_uv[1].y * contribs[1]
			+ m_uv[2].y * contribs[2]);
}

Vector3F Triangle::interpolateColor(const float * contribs) const
{
	return (C(0) * contribs[0] 
		+ C(1) * contribs[1] 
		+ C(2) * contribs[2]);
}

const Float2 & Triangle::uv(const int & i) const
{ return m_uv[i]; }

Quad::Quad()
{}

void Quad::set(const Vector3F & p0, const Vector3F & p1,
			const Vector3F & p2, const Vector3F & p3)
{
	m_p[0] = p0;
	m_p[1] = p1;
	m_p[2] = p2;
	m_p[3] = p3;
}

const Vector3F * Quad::p(int idx) const
{ return &m_p[idx]; }

const Vector3F & Quad::P(int idx) const
{ return m_p[idx]; }

Vector3F Quad::calculateNormal() const
{
	Vector3F r = (m_p[2] - m_p[0]).cross(m_p[3] - m_p[1]);
	return r.normal();
}
	
ShapeType Quad::ShapeTypeId = TQuad;

std::string Quad::GetTypeStr()
{ return "quad"; }

Tetrahedron::Tetrahedron()
{}

void Tetrahedron::set(const Vector3F & p0, const Vector3F & p1,
			const Vector3F & p2, const Vector3F & p3)
{
	m_p[0] = p0; 
	m_p[1] = p1; 
	m_p[2] = p2; 
	m_p[3] = p3; 
}

BoundingBox Tetrahedron::calculateBBox() const
{ 
	BoundingBox b;
    b.expandBy(m_p[0]);
    b.expandBy(m_p[1]);
	b.expandBy(m_p[2]);
	b.expandBy(m_p[3]);
    return b;
}

ShapeType Tetrahedron::ShapeTypeId = TTetrahedron;

std::string Tetrahedron::GetTypeStr()
{ return "tetrahedron"; }

int Tetrahedron::numPoints() const
{ return 4; }

const Vector3F & Tetrahedron::X(int idx) const
{ return m_p[idx]; }

Vector3F Tetrahedron::supportPoint(const Vector3F & v, Vector3F * localP) const
{
	float maxdotv = -1e19f;
    float dotv;
	int ir = 0;
	
    for(int i=0; i < 4; ++i) {
        dotv = X(i).dot(v);
        if(dotv > maxdotv) {
            maxdotv = dotv;
            ir = i;
        }
    }
    if(localP) *localP = X(ir);
    return X(ir);
}

Vector3F Tetrahedron::getCenter() const
{ return (m_p[0] + m_p[1] + m_p[2] + m_p[3]) * .25f; }

Vector3F Tetrahedron::getFaceCenter(const int & i) const
{
	if(i<2)
		return (m_p[0] + m_p[i+1] + m_p[i+2]) * .333333f;
	if(i==2)
		return (m_p[0] + m_p[3] + m_p[1]) * .333333f;
		
	return (m_p[1] + m_p[3] + m_p[2]) * .333333f;
}

void Tetrahedron::getFace(Triangle & tri, const int & i) const
{
	if(i<2)
		return tri.set(m_p[0], m_p[i+1], m_p[i+2]);
	if(i==2)
		return tri.set(m_p[0], m_p[3], m_p[1]);
		
	return tri.set(m_p[1], m_p[3], m_p[2]);
}

Vector3F Tetrahedron::getEdgeCenter(const int & i) const
{
	if(i<3) {
		return (m_p[0] + m_p[i+1]) * .5f;
	}
	if(i<5) {
		return (m_p[1] + m_p[i-1]) * .5f;
	}
	return (m_p[2] + m_p[3]) * .5f;
}

bool Tetrahedron::isPointInside(const Vector3F & q) const
{
    return pointInsideTetrahedronTest(q, m_p);
}

void Tetrahedron::circumSphere(Vector3F & center, float & radius) const
{
    const Vector3F & p1 = m_p[0];
    const Vector3F & p2 = m_p[1];
    const Vector3F & p3 = m_p[2];
    const Vector3F & p4 = m_p[3];
    
    Matrix44F a;
	*a.m(0,0) = p1.x; *a.m(0,1) = p1.y; *a.m(0,2) = p1.z; *a.m(0,3) = 1.f;
	*a.m(1,0) = p2.x; *a.m(1,1) = p2.y; *a.m(1,2) = p2.z; *a.m(1,3) = 1.f;
	*a.m(2,0) = p3.x; *a.m(2,1) = p3.y; *a.m(2,2) = p3.z; *a.m(2,3) = 1.f;
	*a.m(3,0) = p4.x; *a.m(3,1) = p4.y; *a.m(3,2) = p4.z; *a.m(3,3) = 1.f;
	float da = 2.f * a.determinant();
	
	float d1 = p1.length2();
	float d2 = p2.length2();
	float d3 = p3.length2();
	float d4 = p4.length2();
	
	*a.m(0,0) = d1;
	*a.m(1,0) = d2;
	*a.m(2,0) = d3;
	*a.m(3,0) = d4;
	
	float dx = a.determinant();
	
	*a.m(0,1) = p1.x; *a.m(0,2) = p1.z;
	*a.m(1,1) = p2.x; *a.m(1,2) = p2.z;
	*a.m(2,1) = p3.x; *a.m(2,2) = p3.z;
	*a.m(3,1) = p4.x; *a.m(3,2) = p4.z;
	
	float dy = -a.determinant();
	
	*a.m(0,1) = p1.x; *a.m(0,2) = p1.y;
	*a.m(1,1) = p2.x; *a.m(1,2) = p2.y;
	*a.m(2,1) = p3.x; *a.m(2,2) = p3.y;
	*a.m(3,1) = p4.x; *a.m(3,2) = p4.y;
	
	float dz = a.determinant();
	
	center.x = dx / da;
	center.y = dy / da;
	center.z = dz / da;
	radius = p1.distanceTo(center);
}

void Tetrahedron::getCenterRadius(Vector3F & center, float & radius) const
{
	center = getCenter();
	radius = m_p[0].distanceTo(center);
}

const Vector3F & Tetrahedron::P(int idx) const
{ return m_p[idx]; }

}

}
//;~