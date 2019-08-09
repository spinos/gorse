/*
 * Polygon2D.cpp
 * aloe
 *
 *
 * 2019/8/8
 */

#include "Polygon2D.h"
#include "line_math.h"

namespace alo {

Polygon2D::Polygon2D()
{}

Polygon2D::~Polygon2D()
{}

void Polygon2D::reset()
{ m_points.purgeBuffer(); }

void Polygon2D::operator<<(const Float2 &p)
{ m_points<<p; }

int Polygon2D::numSegments() const
{ 
	int r = 0;
	if(m_points.count() > 2) r = m_points.count();
	return r; 
}

void Polygon2D::finish()
{
	std::cout << "\n Polygon2D::finish n v" << m_points.count();
	Float2 cen(0.f, 0.f);
	for(int i=0;i<m_points.count();++i) {
		cen.x += m_points[i].x;
		cen.y += m_points[i].y;
	}
	cen.x *= 1.f / m_points.count();
	cen.y *= 1.f / m_points.count();
	std::cout << " center " << cen.x << ", "<< cen.y;
}

bool Polygon2D::isValid() const
{ return m_points.count() > 2; }

void Polygon2D::getSegment(Float2 &p0, Float2 &p1, const int &i) const
{
	p0 = m_points[i];
	int i1 = i + 1;
	if(i1 > m_points.count() - 1) i1 = 0;
	p1 = m_points[i1];
}

float Polygon2D::angleBetween(const Float2 &p0, const Float2 &p1,
						const Float2 &q) const
{
	Vector3F v0(p0.x, 0.f, p0.y);
	Vector3F v1(p1.x, 0.f, p1.y);
	Vector3F vq(q.x, 0.f, q.y);
	float d;
	distancePointLineSegment(d, vq, v0, v1);
	if(d < 1e-3f) return 0.f;

	v0 -= vq;
	v0.normalize();
	v1 -= vq; 
	v1.normalize();

	const Vector3F c = v0.cross(v1);
	if(c.length2() < 1e-6f) return 0.f;

	float a = acos(v0.dot(v1));
	if(c.y < 0.f) a = -a;

	return a;
}

bool Polygon2D::isPointInside(const Float2 &q) const
{
	Float2 p0, p1;
	float s = 0.f;
	const int n = numSegments();
	for(int i=0;i<n;++i) {
		getSegment(p0, p1, i);
		float a = angleBetween(p0, p1, q);
		s += a;
	}

	return s > 6.28f;
}

}
