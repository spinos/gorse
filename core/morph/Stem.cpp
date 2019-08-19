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
m_parent(nullptr),
m_node0Ang(0.f)
{ m_tube = new TubularProfile; }

Stem::~Stem()
{ delete m_tube; }

void Stem::setParent(const Stem *x)
{ m_parent = x; }

void Stem::begin(const Vector3F &pos, const Matrix33F &mat,
				const float &a0, const float &radius0)
{ 
	m_tube->begin(pos, mat, radius0); 
	m_node0Ang = a0;
	m_age = 0;
    m_nodeIndex.clear();
    m_segmentAngles.clear();
}

void Stem::grow(const Vector3F &gv, const float &dWidth, StemProfile &stp)
{
	const Vector3F gvn = gv.normal();
	const int &n = stp.segmentsPerSeason();
	const float l = gv.length() / (float)n;
	const float d = dWidth / (float)n;
	for(int i=0;i<n;++i) {
		*m_tube << getGrowDirection(gvn, stp) * l;
		m_tube->addRadius(d);
        
        m_segmentAngles.push_back(stp.segmentAngle());
	}
	m_age++;
    m_nodeIndex.push_back(m_tube->numSegments() - 1);
}

void Stem::end()
{ m_tube->end(); }

const TubularProfile *Stem::profile() const
{ return m_tube; }

const Stem *Stem::parent() const
{ return m_parent; }

const int &Stem::age() const
{ return m_age; }

void Stem::getTerminalBud(Vector3F &pos, Matrix33F &mat, StemProfile &stp) const
{
	pos = m_tube->pos0();
	mat = m_tube->frm0();
    Matrix33F::rotateAroundLocalX(mat, m_node0Ang);
	const int n = m_tube->numSegments();
	for(int i=0;i<n;++i) {
		const Vector3F &vi = m_tube->displacement(i);
		pos += vi;
		Matrix33F::rotateToAlign(mat, vi);
        Matrix33F::rotateAroundLocalX(mat, stp.segmentAngle());
	}
}

void Stem::getTerminalBudRotation(Matrix33F &mat, StemProfile &stp) const
{
	mat = m_tube->frm0();
	Matrix33F::rotateAroundLocalX(mat, m_node0Ang);
	const int n = m_tube->numSegments();
	for(int i=0;i<n;++i) {
		const Vector3F &vi = m_tube->displacement(i);
		Matrix33F::rotateToAlign(mat, vi);
		Matrix33F::rotateAroundLocalX(mat, stp.segmentAngle());
	}
}

Vector3F Stem::getGrowDirection(const Vector3F &ref, StemProfile &stp) const
{
	Matrix33F mat;
	getTerminalBudRotation(mat, stp);
	Matrix33F::rotateToAlignLimited(mat, ref, stp.bendingLimit());
	return Vector3F(mat.M(0, 0), mat.M(0, 1), mat.M(0, 2));
}

const float &Stem::radius0() const
{ return m_tube->radius(0); }

void Stem::getAllBuds(std::vector<Vector3F> &positions, std::vector<Matrix33F> &rotations) const
{
    Vector3F pos = m_tube->pos0();
	Matrix33F mat = m_tube->frm0();
    Matrix33F::rotateAroundLocalX(mat, m_node0Ang);
	const int n = m_tube->numSegments();
    
    int curNode = 0;
	for(int i=0;i<n;++i) {
		const Vector3F &vi = m_tube->displacement(i);
		pos += vi;
		Matrix33F::rotateToAlign(mat, vi);
        Matrix33F::rotateAroundLocalX(mat, m_segmentAngles[i]);
        
        if(i == m_nodeIndex[curNode]) {
            positions.push_back(pos);
            rotations.push_back(mat);
            curNode++;
        }
	}
}

}

}
