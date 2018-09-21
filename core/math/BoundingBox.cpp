/*
 *  BoundingBox.cpp
 *  kdtree
 *
 *  Created by jian zhang on 10/17/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#include "BoundingBox.h"
#include <math/Ray.h>
#include <math/miscfuncs.h>

namespace alo {

BoundingBox::BoundingBox()
{
	reset();
}

BoundingBox::BoundingBox(const float & x0, const float & y0, const float & z0,
	            const float & x1, const float & y1, const float & z1)
{
    m_data[0] = x0;
    m_data[1] = y0;
    m_data[2] = z0;
    m_data[3] = x1;
    m_data[4] = y1;
    m_data[5] = z1;
}

BoundingBox::BoundingBox(const float * d)
{
	memcpy(m_data, d, 24);
}

void BoundingBox::setCenterHalfSpan(const float* & c)
{
	const float& h = c[3];
	m_data[0] = c[0] - h;
	m_data[1] = c[1] - h;
	m_data[2] = c[2] - h;
	m_data[3] = c[0] + h;
	m_data[4] = c[1] + h;
	m_data[5] = c[2] + h;
}

void BoundingBox::set(const Vector3F & center,
			const float & h)
{
	m_data[0] = center.x - h;
	m_data[1] = center.y - h;
	m_data[2] = center.z - h;
	m_data[3] = center.x + h;
	m_data[4] = center.y + h;
	m_data[5] = center.z + h;
}

void BoundingBox::reset()
{
	m_data[0] = m_data[1] = m_data[2] = 10e8f;
	m_data[3] = m_data[4] = m_data[5] = -10e8f;
}

void BoundingBox::setOne()
{
	setMin(-1.f, -1.f, -1.f);
	setMax(1.f, 1.f, 1.f);
}

void BoundingBox::setMin(float x, int axis)
{
	m_data[axis] = x;
}
	
void BoundingBox::setMax(float x, int axis)
{
	m_data[axis + 3] = x;
}

void BoundingBox::setMin(float x, float y, float z)
{
	m_data[0] = x; m_data[1] = y; m_data[2] = z;
}

void BoundingBox::setMax(float x, float y, float z)
{
	m_data[3] = x; m_data[4] = y; m_data[5] = z;
}

void BoundingBox::update(const Vector3F & p)
{
	updateMin(p);
	updateMax(p);
}

void BoundingBox::updateMin(const Vector3F & p)
{
	if(m_data[0] > p.x) m_data[0] = p.x;
	if(m_data[1] > p.y) m_data[1] = p.y;
	if(m_data[2] > p.z) m_data[2] = p.z;
}

void BoundingBox::updateMax(const Vector3F & p)
{
	if(m_data[3] < p.x) m_data[3] = p.x;
	if(m_data[4] < p.y) m_data[4] = p.y;
	if(m_data[5] < p.z) m_data[5] = p.z;
}

const int BoundingBox::getLongestAxis() const
{
	Vector3F d(m_data[3] - m_data[0], m_data[4] - m_data[1], m_data[5] - m_data[2]);
	if(d.y >= d.x && d.y >= d.z) return 1;
	if(d.z >= d.x && d.z >= d.y) return 2;
	return 0;
}

const float BoundingBox::getLongestDistance() const
{
    Vector3F d(m_data[3] - m_data[0], m_data[4] - m_data[1], m_data[5] - m_data[2]);
	if(d.y >= d.x && d.y >= d.z) return d.y;
	if(d.z >= d.x && d.z >= d.y) return d.z;
	return d.x;
}

const float BoundingBox::getMin(int axis) const
{
	return m_data[axis];
}

const float BoundingBox::getMax(int axis) const
{
	return m_data[axis + 3];
}

const Vector3F BoundingBox::getMin() const { return Vector3F(m_data[0], m_data[1], m_data[2]); }
const Vector3F BoundingBox::getMax() const { return Vector3F(m_data[3], m_data[4], m_data[5]); }

const float BoundingBox::area() const
{
	return ((m_data[3] - m_data[0]) * (m_data[4] - m_data[1]) + (m_data[3] - m_data[0]) * (m_data[5] - m_data[2]) + (m_data[4] - m_data[1]) * (m_data[5] - m_data[2])) * 2.f;
}

const float BoundingBox::crossSectionArea(const int &axis) const
{
	if(axis == 0) {
		return (m_data[4] - m_data[1]) * (m_data[5] - m_data[2]);
	}
	if(axis == 1) {
		return (m_data[3] - m_data[0]) * (m_data[5] - m_data[2]);
	}
	return (m_data[3] - m_data[0]) * (m_data[4] - m_data[1]);
}

const float BoundingBox::distance(const int &axis) const
{
	return m_data[axis + 3] - m_data[axis];
}

void BoundingBox::split(int axis, float pos, BoundingBox & lft, BoundingBox & rgt) const
{
	lft = rgt = *this;
	
	lft.setMax(pos, axis);
	rgt.setMin(pos, axis);
}

void BoundingBox::splitLeft(int axis, float pos, BoundingBox & lft) const
{
	lft = *this;
	lft.setMax(pos, axis);
}

void BoundingBox::splitRight(int axis, float pos, BoundingBox & rgt) const
{
	rgt = *this;
	rgt.setMin(pos, axis);
}

void BoundingBox::expandBy(const BoundingBox &another)
{
	if(m_data[0] > another.m_data[0]) m_data[0] = another.m_data[0];
	if(m_data[1] > another.m_data[1]) m_data[1] = another.m_data[1];
	if(m_data[2] > another.m_data[2]) m_data[2] = another.m_data[2];
	
	if(m_data[3] < another.m_data[3]) m_data[3] = another.m_data[3];
	if(m_data[4] < another.m_data[4]) m_data[4] = another.m_data[4];
	if(m_data[5] < another.m_data[5]) m_data[5] = another.m_data[5];
}

void BoundingBox::expandBy(const Vector3F & pos)
{
    if(m_data[0] > pos.x) m_data[0] = pos.x;
    if(m_data[1] > pos.y) m_data[1] = pos.y;
    if(m_data[2] > pos.z) m_data[2] = pos.z;
    if(m_data[3] < pos.x) m_data[3] = pos.x;
    if(m_data[4] < pos.y) m_data[4] = pos.y;
    if(m_data[5] < pos.z) m_data[5] = pos.z;
}

void BoundingBox::expandBy(const Vector3F & pos, float r)
{
    if(m_data[0] > pos.x - r) m_data[0] = pos.x - r;
    if(m_data[1] > pos.y - r) m_data[1] = pos.y - r;
    if(m_data[2] > pos.z - r) m_data[2] = pos.z - r;
    if(m_data[3] < pos.x + r) m_data[3] = pos.x + r;
    if(m_data[4] < pos.y + r) m_data[4] = pos.y + r;
    if(m_data[5] < pos.z + r) m_data[5] = pos.z + r;
}

void BoundingBox::shrinkBy(const BoundingBox &another)
{
    if(m_data[0] < another.m_data[0]) m_data[0] = another.m_data[0];
	if(m_data[1] < another.m_data[1]) m_data[1] = another.m_data[1];
	if(m_data[2] < another.m_data[2]) m_data[2] = another.m_data[2];
	
	if(m_data[3] > another.m_data[3]) m_data[3] = another.m_data[3];
	if(m_data[4] > another.m_data[4]) m_data[4] = another.m_data[4];
	if(m_data[5] > another.m_data[5]) m_data[5] = another.m_data[5];
}

void BoundingBox::expand(float val)
{
    m_data[0] -= val;
    m_data[1] -= val;
    m_data[2] -= val;
    m_data[3] += val;
    m_data[4] += val;
    m_data[5] += val;
}

Vector3F BoundingBox::center() const
{
	return Vector3F((m_data[0] + m_data[3]) * 0.5f, 
					(m_data[1] + m_data[4]) * 0.5f, 
					(m_data[2] + m_data[5]) * 0.5f);
}

Vector3F BoundingBox::lowCorner() const
{
	return Vector3F(m_data[0], m_data[1], m_data[2]);
}

Vector3F BoundingBox::highCorner() const
{
	return Vector3F(m_data[3], m_data[4], m_data[5]);
}

char BoundingBox::touch(const BoundingBox & b) const
{
	if(m_data[0] >= b.m_data[3] || m_data[3] <= b.m_data[0]) return 0;
	if(m_data[1] >= b.m_data[4] || m_data[4] <= b.m_data[1]) return 0;
	if(m_data[2] >= b.m_data[5] || m_data[5] <= b.m_data[2]) return 0;
	return 1;
}

float BoundingBox::distanceTo(const Vector3F & pnt) const
{
    if(isPointInside(pnt)) return -1.f;
    
    float dx = getMin(0) - pnt.x;
    dx = dx > 0.f ? dx : 0.f;
    const float dx1 = pnt.x - getMax(0);
    dx = dx > dx1 ? dx : dx1;
    
    float dy = getMin(1) - pnt.y;
    dy = dy > 0.f ? dy : 0.f;
    const float dy1 = pnt.y - getMax(1);
    dy = dy > dy1 ? dy : dy1;
    
    float dz = getMin(2) - pnt.z;
    dz = dz > 0.f ? dz : 0.f;
    const float dz1 = pnt.z - getMax(2);
    dz = dz > dz1 ? dz : dz1;
    
    return sqrt(dx * dx + dy * dy + dz * dz);
}

bool BoundingBox::intersect(const BoundingBox & another) const
{
    if(getMin(0) >= another.getMax(0)) return 0;
	if(getMax(0) <= another.getMin(0)) return 0;
	if(getMin(1) >= another.getMax(1)) return 0;
	if(getMax(1) <= another.getMin(1)) return 0; 
	if(getMin(2) >= another.getMax(2)) return 0;
	if(getMax(2) <= another.getMin(2)) return 0;    
    return 1;
}

bool BoundingBox::intersect(const BoundingBox & another, BoundingBox * tightBox) const
{
	float a, b, c, d;
	for(int i=0; i < 3; i++) {
		a = getMin(i);
		b = getMax(i);
		c = another.getMin(i);
		d = another.getMax(i);
		if(a > d) return false;
        if(b < c) return false;
/// higher of min
		if(a < c) a = c;
		tightBox->setMin(a, i);
/// lower of max		
		if(b > d) b = d;
		tightBox->setMax(b, i);    
	}
	return true;
}

char BoundingBox::intersect(const Ray &ray, float *hitt0, float *hitt1) const 
{		
    float t0 = ray.m_tmin, t1 = ray.m_tmax;
    for (int i = 0; i < 3; ++i) {
		const float diri = ray.m_dir.comp(i);
		const Vector3F o = ray.m_origin;
		if(IsValueNearZero(diri)) {
			if(i == 0) {
				if(o.x <= m_data[0] || o.x >= m_data[3]) return 0;
			}
			else if(i == 1) {
				if(o.y <= m_data[1] || o.y >= m_data[4]) return 0;
			}
			else {
				if(o.z <= m_data[2] || o.z >= m_data[5]) return 0;
			}
			continue;
		}
        // Update interval for _i_th bounding box slab
        float invRayDir = 1.f / (ray.m_dir.comp(i) + 1e-9f);
        float tNear = (getMin(i) - ray.m_origin.comp(i)) * invRayDir;
        float tFar  = (getMax(i) - ray.m_origin.comp(i)) * invRayDir;

        // Update parametric interval from slab intersection $t$s
        if (tNear > tFar) SwapValues(tNear, tFar);
        t0 = tNear > t0 ? tNear : t0;
        t1 = tFar  < t1 ? tFar  : t1;
        if (t0 > t1) return 0;
    }
    if (hitt0) *hitt0 = t0;
    if (hitt1) *hitt1 = t1;
    return 1;
}

char BoundingBox::isPointInside(const Vector3F & p) const
{
	if(p.x < getMin(0) || p.x > getMax(0)) return 0;
	if(p.y < getMin(1) || p.y > getMax(1)) return 0;
	if(p.z < getMin(2) || p.z > getMax(2)) return 0;
	return 1;
}

char BoundingBox::isPointAround(const Vector3F & p, float threshold) const
{
	if(p.x + threshold < getMin(0) || p.x - threshold > getMax(0)) return 0;
	if(p.y + threshold < getMin(1) || p.y - threshold > getMax(1)) return 0;
	if(p.z + threshold < getMin(2) || p.z - threshold > getMax(2)) return 0;
	return 1;
}

char BoundingBox::isBoxAround(const BoundingBox & b, float threshold) const
{
    if(b.getMin(0) > getMax(0) + threshold) return 0;
    if(b.getMax(0) < getMin(0) - threshold) return 0;
    if(b.getMin(1) > getMax(1) + threshold) return 0;
    if(b.getMax(1) < getMin(1) - threshold) return 0;
    if(b.getMin(2) > getMax(2) + threshold) return 0;
    if(b.getMax(2) < getMin(2) - threshold) return 0;
    return 1;
}

char BoundingBox::inside(const BoundingBox & b) const
{
	return (getMin(0) >= b.getMin(0) &&
			getMin(1) >= b.getMin(1) &&
			getMin(2) >= b.getMin(2) &&
			getMax(0) <= b.getMax(0) &&
			getMax(1) <= b.getMax(1) &&
			getMax(2) <= b.getMax(2));
}

char BoundingBox::isValid() const
{
	return (getMin(0) < getMax(0) && getMin(1) < getMax(1) && getMin(2) < getMax(2));
}

/*
// z0
// 2 - 3
// |   |
// 0 - 1
// 
// z1
// 6 - 7
// |   |
// 4 - 5
*/

const Vector3F BoundingBox::corner(const int & i) const
{
    const int iz = i / 4;
    const int iy = (i - iz * 4) / 2;
    const int ix = i - iy * 2 - iz * 4;
    return Vector3F(m_data[3 * ix], m_data[1 + 3 * iy], m_data[2 + 3 * iz]);
}

const Vector3F BoundingBox::normal(const int & i) const
{
    if(i<1) return Vector3F::XAxis;
    if(i<2) return Vector3F::YAxis;
    return Vector3F::ZAxis;
}

float BoundingBox::radius() const
{
    float dx = distance(0) * .5f;
    float dy = distance(1) * .5f;
    float dz = distance(2) * .5f;
    return sqrt(dx*dx + dy*dy + dz*dz);
}

float BoundingBox::radiusXZ() const
{
	float dx = distance(0) * .5f;
    float dz = distance(2) * .5f;
    return sqrt(dx*dx + dz*dz);
}

int BoundingBox::pointOnEdge(const Vector3F & v) const
{
	Vector3F r = v - center();
	r.x /= distance(0);
	r.y /= distance(1);
	r.z /= distance(2);
	r.normalize();
	int jr = r.longestAxis();
	if(jr == 0) {
		if(r.x == r.y || r.x == r.z) {
			if(r.x < 0.f) return 0;
			return 1;
		}
		return -1;
	}
	if(jr == 1) {
		if(r.y == r.x || r.y == r.z) {
			if(r.y < 0.f) return 2;
			return 3;
		}
		return -1;
	}
	if(r.z == r.x || r.z == r.y) {
		if(r.z < 0.f) return 4;
		return 5;
	}
	return -1;
}

/// should not on edge/corner
int BoundingBox::pointOnSide(const Vector3F & v) const
{
	Vector3F r = v - center();
	r.x /= distance(0);
	r.y /= distance(1);
	r.z /= distance(2);
	//r.normalize();
	int jr = r.longestAxis();
	if(jr == 0) {
		if(r.x < 0.f) return 0;
		return 1;
	}
	if(jr == 1) {
		if(r.y < 0.f) return 2;
		return 3;
	}
	if(r.z < 0.f) return 4;
	return 5;
}

int BoundingBox::numPoints() const
{ return 8; }

Vector3F BoundingBox::X(int idx) const
{
    Vector3F r(m_data[0], m_data[1], m_data[2]);
    if(idx > 3) r.z = m_data[5];
    if((idx & 3) > 1) r.y = m_data[4];
    if(idx & 1) r.x = m_data[3];
    return r;
}

Vector3F BoundingBox::supportPoint(const Vector3F & v, Vector3F * localP) const
{
    float maxdotv = -1e18f;
    float dotv;
	
    Vector3F res, q;
    for(int i=0; i < numPoints(); i++) {
        q = X(i);
        dotv = q.dot(v);
        if(dotv > maxdotv) {
            maxdotv = dotv;
            res = q;
            if(localP) *localP = q;
        }
    }
    
    return res;
}

const float * BoundingBox::data() const
{ return m_data; }

const float BoundingBox::volume() const
{ return distance(0) * distance(1) * distance(2); }

void BoundingBox::round()
{
	if(m_data[0] < 0.f) m_data[0] = -(int(-m_data[0]) + 1);
	else m_data[0] = int(m_data[0]);
	
	if(m_data[1] < 0.f) m_data[1] = -(int(-m_data[1]) + 1);
	else m_data[1] = int(m_data[1]);
	
	if(m_data[2] < 0.f) m_data[2] = -(int(-m_data[2]) + 1);
	else m_data[2] = int(m_data[2]);
	
	if(m_data[3] < 0.f) m_data[3] = -(int(-m_data[3]) );
	else m_data[3] = int(m_data[3]) + 1;
	
	if(m_data[4] < 0.f) m_data[4] = -(int(-m_data[4]) );
	else m_data[4] = int(m_data[4]) + 1;
	
	if(m_data[5] < 0.f) m_data[5] = -(int(-m_data[5]) );
	else m_data[5] = int(m_data[5]) + 1;
}

void BoundingBox::translate(const Vector3F & ta)
{
	m_data[0] += ta.x;
	m_data[3] += ta.x;
	m_data[1] += ta.y;
	m_data[4] += ta.y;
	m_data[2] += ta.z;
	m_data[5] += ta.z;
}

void BoundingBox::translate(const float * ta)
{
	m_data[0] += ta[0];
	m_data[3] += ta[0];
	m_data[1] += ta[1];
	m_data[4] += ta[1];
	m_data[2] += ta[2];
	m_data[5] += ta[2];
}

void BoundingBox::scale(const float & sc)
{
	m_data[0] *= sc;
	m_data[1] *= sc;
	m_data[2] *= sc;
	m_data[3] *= sc;
	m_data[4] *= sc;
	m_data[5] *= sc;
}

void BoundingBox::scale(const float * sc)
{
	m_data[0] *= sc[0];
	m_data[1] *= sc[1];
	m_data[2] *= sc[2];
	m_data[3] *= sc[0];
	m_data[4] *= sc[1];
	m_data[5] *= sc[2];
}

const BoundingBox & BoundingBox::calculateBBox() const
{ return *this; }

int BoundingBox::numFlatAxis(const float & threshold) const
{
	int c = 0;
	if(distance(0) < threshold)
		c++;
		
	if(distance(1) < threshold)
		c++;
		
	if(distance(2) < threshold)
		c++;
	
	return c;
}

void BoundingBox::getSizeOrder(int * d) const
{
	float lx = distance(0);
	float ly = distance(1);
	float lz = distance(2);
/// separate
	if(lx == ly) {
		lx += 1e-4f;
	}
	
	if(lx == lz) {
		lx += 1e-4f;
		lz -= 1e-4f;
	}
	
	if(lz == ly) {
		lz -= 1e-4f;
	}
	
	d[0] = 0;
	if(ly > lx && ly > lz) {
		d[0] = 1;
	}
	if(lz > lx && lz > ly) {
		d[0] = 2;
	}
	d[2] = 2;
	if(lx < ly && lx < lz) {
		d[2] = 0;
	}
	if(ly < lx && ly < lz) {
		d[2] = 1;
	}
	
	d[1] = 1;
	if(d[0] != 0 && d[2] != 0) {
		d[1] = 0;
	}
	if(d[0] != 2 && d[2] != 2) {
		d[1] = 2;
	}
}

void BoundingBox::putInside(Vector3F & p) const
{
	if(p.x < getMin(0)) p.x = getMin(0);
	if(p.x > getMax(0)) p.x = getMax(0);
	if(p.y < getMin(1)) p.y = getMin(1);
	if(p.y > getMax(1)) p.y = getMax(1);
	if(p.z < getMin(2)) p.z = getMin(2);
	if(p.z > getMax(2)) p.z = getMax(2);
}

void BoundingBox::getSubBox(BoundingBox & dst,
				const Vector3F & coord,
				const float & delta) const
{
	const float d0 = distance(0);
	dst.m_data[0] = m_data[0] + d0 * coord.x;
	dst.m_data[3] = dst.m_data[0] + d0 * delta;
	
	const float d1 = distance(1);
	dst.m_data[1] = m_data[1] + d1 * coord.y;
	dst.m_data[4] = dst.m_data[1] + d1 * delta;
	
	const float d2 = distance(2);
	dst.m_data[2] = m_data[2] + d2 * coord.z;
	dst.m_data[5] = dst.m_data[2] + d2 * delta;
}

bool BoundingBox::isBoxOnBoundary(const BoundingBox & b) const
{
	if(b.m_data[0] <= m_data[0]) {
		return  true;
	}
	if(b.m_data[1] <= m_data[1]) {
		return  true;
	}
	if(b.m_data[2] <= m_data[2]) {
		return  true;
	}
	if(b.m_data[3] >= m_data[3]) {
		return  true;
	}
	if(b.m_data[4] >= m_data[4]) {
		return  true;
	}
	if(b.m_data[5] >= m_data[5]) {
		return  true;
	}
	return false;
}

std::ostream& operator<<(std::ostream &output, const BoundingBox & p) 
{
    output<<"(("<<p.m_data[0]<<","<<p.m_data[1]<<","<<p.m_data[2]
    <<"),("<<p.m_data[3]<<","<<p.m_data[4]<<","<<p.m_data[5]<<"))";
	
    return output;
}

}
//:~