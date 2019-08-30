/* 
 *  Stem.cpp
 *  gorse
 *
 *  2019/8/17
 */

#include "Stem.h"
#include <tbl/TubularProfile.h>
#include "StemProfile.h"
#include <tbl/TubularCrossSection.h>

namespace alo {

namespace morph {

Stem::Stem() :
m_parent(nullptr),
m_node0Ang(0.f),
m_isStopped(false),
m_isBranched(false),
m_ageOffset(0)
{ m_tube = new TubularProfile; }

Stem::~Stem()
{ delete m_tube; }

void Stem::setParent(const Stem *x)
{ m_parent = x; }

void Stem::addChild(Stem *x)
{ m_children.push_back(x); }

void Stem::begin(const Vector3F &pos, const Matrix33F &mat,
				const float &a0, const float &radius0)
{ 
	m_tube->begin(pos, mat, radius0); 
	m_node0Ang = a0;
	m_age = 0;
    m_nodeIndex.clear();
    m_segmentAngles.clear();
    m_radiusChanges.clear();
}

void Stem::grow(const Vector3F &gv, const float &dWidth, StemProfile &stp)
{
    if(m_isStopped)
        return growThicker(dWidth, stp);

	const Vector3F gvn = getGeneralDirection(gv, stp);
	const int &n = stp.segmentsPerSeason();
	const float l = gv.length() / (float)n;
	const float d = dWidth / (float)n;
	for(int i=0;i<n;++i) {
		*m_tube << getGrowDirection(gvn, stp) * l;
		m_tube->addRadius(d);
        
        m_radiusChanges.push_back(d);
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

int Stem::numChildren() const
{ return m_children.size(); }

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

const bool &Stem::isStopped() const
{ return m_isStopped; }

void Stem::setStopped()
{ m_isStopped = true; }

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

void Stem::growThicker(const float &dWidth, StemProfile &stp)
{
    m_tube->addRadius(dWidth);
    
    const int &n = stp.segmentsPerSeason();
	const float d = dWidth / (float)n;
	for(int i=0;i<n;++i) {
        m_radiusChanges.push_back(d);
	}
    
    m_age++;
}

void Stem::getResourceFlow(float &res, const float &ratio) const
{
    const Stem *pst = parent();
    while(pst) {
        float domin = pst->isStopped() ? 1.f : ratio;
        res *= domin / (float)pst->numChildren();
        pst = pst->parent();
    }
    
}

void Stem::setBranched(const bool &x)
{ m_isBranched = x; }

const bool &Stem::isBranched() const
{ return m_isBranched; }

const Matrix33F &Stem::germinatedSpace() const
{ return m_tube->frm0(); }

Vector3F Stem::getGeneralDirection(const Vector3F &gv, StemProfile &stp) const
{ 
    if(!isBranched()) return gv.normal(); 
    
    Matrix33F mat;
	getTerminalBudRotation(mat, stp);
    
    const Stem *lastChild = m_children.back();
    const Matrix33F &childMat = lastChild->germinatedSpace();
    Vector3F ref = Vector3F(mat.M(0, 0), mat.M(0, 1), mat.M(0, 2)).cross(Vector3F(childMat.M(0, 0), childMat.M(0, 1), childMat.M(0, 2)));
    ref.normalize();
    mat *= Matrix33F(Quaternion(-1.f * stp.axilAngle(), ref));
	return Vector3F(mat.M(0, 0), mat.M(0, 1), mat.M(0, 2));
}

void Stem::setAgeOffset(const int &x)
{ m_ageOffset = x; }

const int &Stem::ageOffset() const
{ return m_ageOffset; }

float Stem::getSegment(const float &fage, StemProfile &stp) const
{
    float res = fage - m_ageOffset;
    res *= stp.segmentsPerSeason();
    
    return res;
}

void Stem::getCrossSection(TubularCrossSection *tucrs,
                    StemProfile *stp) const
{
    float r = radius0();
    if(stp) {
        const float &fseg = stp->currentSegment();
        getSegmentRadius(r, fseg);
                
    }
    tucrs->create(10, r, r);
}

void Stem::getSegmentRadius(float &res, const float &fseg) const
{
    if(fseg > m_radiusChanges.size()) return;
    const int n = m_radiusChanges.size();
    const int last = fseg;
    for(int i=n-1;i>=last;--i) {
        float d = m_radiusChanges[i];
        if(i==last) {
            d *= 1.f - (fseg - last);
        }
        res -= d;
    }
    
}

}

}
