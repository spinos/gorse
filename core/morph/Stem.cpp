/* 
 *  Stem.cpp
 *  gorse
 *
 *  2019/8/17
 */

#include "Stem.h"
#include <tbl/TubularProfile.h>
#include "StemProfile.h"

namespace alo {

namespace morph {

Stem::Stem() :
m_node0Ang(0.f)
{ m_tube = new TubularProfile; }

Stem::~Stem()
{ delete m_tube; }

void Stem::begin(const Vector3F &pos, const Matrix33F &mat,
				const float &a0, const float &radius0)
{ 
	m_tube->begin(pos, mat); 
	m_node0Ang = a0;
	m_age = 0;
	m_radius = radius0;
}

void Stem::grow(const Vector3F &gv, const float &dWidth, StemProfile &stp)
{
	const Vector3F gvn = gv.normal();
	const int &n = stp.segmentsPerSeason();
	const float l = gv.length() / (float)n;
	const float d = dWidth / (float)n;
	for(int i=0;i<n;++i) {
		*m_tube << getGrowDirection(gvn, stp) * l;
		m_tube->expandRadius(d);
	}
	m_age++;
	m_radius += dWidth;
}

void Stem::end()
{ m_tube->end(); }

const TubularProfile *Stem::profile() const
{ return m_tube; }

const float &Stem::radius() const
{ return m_radius; }

const int &Stem::age() const
{ return m_age; }

void Stem::getTerminalBud(Vector3F &pos, Matrix33F &mat, StemProfile &stp) const
{
	pos = m_tube->pos0();
	mat = m_tube->frm0();
	float xa = m_node0Ang;
	const int n = m_tube->numSegments();
	for(int i=0;i<n;++i) {
		const Vector3F &vi = m_tube->displacement(i);
		pos += vi;
		Matrix33F::rotateToAlign(mat, vi);
		xa += stp.nodeAngle();
	}
	Matrix33F::rotateAroundLocalX(mat, xa);
}

void Stem::getTerminalBudRotation(Matrix33F &mat, StemProfile &stp) const
{
	mat = m_tube->frm0();
	float xa = m_node0Ang;
	const int n = m_tube->numSegments();
	for(int i=0;i<n;++i) {
		const Vector3F &vi = m_tube->displacement(i);
		Matrix33F::rotateToAlign(mat, vi);
		xa += stp.nodeAngle();
	}
	Matrix33F::rotateAroundLocalX(mat, xa);
}

Vector3F Stem::getGrowDirection(const Vector3F &ref, StemProfile &stp) const
{
	Matrix33F mat;
	getTerminalBudRotation(mat, stp);
	Matrix33F::rotateToAlignLimited(mat, ref, stp.bendingLimit());
	return Vector3F(mat.M(0, 0), mat.M(0, 1), mat.M(0, 2));
}

}

}
