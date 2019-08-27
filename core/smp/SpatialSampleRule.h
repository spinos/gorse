/*
 *  SpatialSampleRule.h
 *  gorse
 *
 *  grid by min distance
 *  select one sample per cell
 *  reject samples close to any selection in neighbor cells
 *  2019/8/21
 */
 
#ifndef ALO_SPATIAL_SAMPLE_RULE_H
#define ALO_SPATIAL_SAMPLE_RULE_H

#include <math/Matrix33F.h>
#include <svf/SvfBuildRule.h>
#include <sdb/L3Tree.h>

namespace alo {
    
template<typename Trng, typename Tc>
class SpatialSampleRule : public svf::SvfBuildRule<Tc> {
    
    Trng *m_rng;
 	int m_dataSize;
	int m_numSelected;
/// limit size of subset much smaller than size of data
	int m_maxNumSelected;
    float m_minDistance;
/// selected position in each cell
    sdb::L3Tree<int, Vector3F, 1024, 512, 512 > m_cellSelPosMap;
    int m_touchedCells[27];
    
    typedef svf::SvfBuildRule<Tc> PRuleTyp;
    
public:

    SpatialSampleRule(Tc *sfc);
	virtual ~SpatialSampleRule();

	void setRng(Trng *x);
	void setMaxNumSelected(int x);
    
    template<typename Ts>
    void createGrid(const Ts *data, const int &n, 
                    const float &minDistance);
/// i-th sample in data
    template<typename Ts>
	bool accept(const Ts &x, const int &i);

	bool finished() const;
    void begin();
    
    const int &numSelected() const;
    
    template<typename Ts>
    Matrix33F getRotation(const Ts &s);

protected:
	
	Trng *rng();
	float getAcceptRation(const int &i, const float &scaling) const;
	bool rejectByPosition(const Vector3F &pos);
	void select(const Vector3F &pos);
	
private:

};

template<typename Trng, typename Tc>
SpatialSampleRule<Trng, Tc>::SpatialSampleRule(Tc *sfc) : svf::SvfBuildRule<Tc>(sfc),
m_rng(nullptr),
m_dataSize(0),
m_numSelected(0),
m_maxNumSelected(100),
m_minDistance(1.f)
{}

template<typename Trng, typename Tc>
SpatialSampleRule<Trng, Tc>::~SpatialSampleRule()
{}

template<typename Trng, typename Tc>
void SpatialSampleRule<Trng, Tc>::setRng(Trng *x)
{ m_rng = x; }

template<typename Trng, typename Tc>
void SpatialSampleRule<Trng, Tc>::setMaxNumSelected(int x)
{ m_maxNumSelected = x; }

template<typename Trng, typename Tc>
template<typename Ts>
void SpatialSampleRule<Trng, Tc>::createGrid(const Ts *data, const int &n, 
                                            const float &minDistance)
{
    m_minDistance = minDistance;
    if(m_minDistance < .1f) m_minDistance = .1f;
    
    BoundingBox b;
	for(int i=0;i<n;++i) {
        b.expandBy(data[i]._pos);
	}
    
    BoundingBox domainBox;
    int d = m_minDistance * 512.f + 1;
    domainBox.set(b.center(), (float)d);
    
    std::cout << "\n cell size " << m_minDistance << "\n domain box " << domainBox;
    
    svf::SvfBuildRule<Tc>::setBBox(domainBox);
	m_dataSize = n;
}

template<typename Trng, typename Tc>
bool SpatialSampleRule<Trng, Tc>::finished() const
{ return m_numSelected >= m_maxNumSelected; }

template<typename Trng, typename Tc>
void SpatialSampleRule<Trng, Tc>::begin()
{ 
	m_numSelected = 0; 
	m_cellSelPosMap.clear();
    for(int i=0;i<27;++i) m_touchedCells[i] = -1;
}

template<typename Trng, typename Tc>
template<typename Ts>
bool SpatialSampleRule<Trng, Tc>::accept(const Ts &x, const int &i)
{
    if(x._nml.y < .7f) return false;
    
	if(m_rng->randf1() > getAcceptRation(i, 3.f)) return false;
    
    const int k = PRuleTyp::computeKey((const float *)&x._pos);
    Vector3F *inCell = m_cellSelPosMap.find(k);
    if(inCell) return false;
    
    if(k != m_touchedCells[13]) {
        PRuleTyp::getLevel27Cells(m_touchedCells, k, 10);
    }
    
    for(int j = 0;j < 27;++j) {
        if(j==13) continue;
        inCell = m_cellSelPosMap.find(m_touchedCells[j]);
        if(!inCell) continue;
        
        if(x._pos.distanceTo(*inCell) < m_minDistance) return false;
    }
    
    m_cellSelPosMap.insert(k, x._pos);
	m_numSelected++;
	return true;
}

template<typename Trng, typename Tc>
const int &SpatialSampleRule<Trng, Tc>::numSelected() const
{ return m_numSelected; }

template<typename Trng, typename Tc>
template<typename Ts>
Matrix33F SpatialSampleRule<Trng, Tc>::getRotation(const Ts &s)
{
	Vector3F up = s._nml; up.normalize();
    Vector3F front(0.f, 0.f, 1.f);
    if(up.z < -.9f || up.z > .9f) front.set(0.f, 1.f, 0.f);
    
	Vector3F side = up.cross(front); side.normalize();
    front = side.cross(up); front.normalize();
	
	Matrix33F r;
	r.fill(side, up, front);
	
    float rollAngle = 6.28f * (m_rng->randf1() - .5f);

	Quaternion rq(rollAngle, up);
	Matrix33F roll(rq);
    r *= roll;
    Matrix33F::rotateUpToAlignLimited(r, Vector3F::YAxis, .1f + m_rng->randf1() * .8f);
    return r;
}

template<typename Trng, typename Tc>
Trng *SpatialSampleRule<Trng, Tc>::rng()
{ return m_rng; }

template<typename Trng, typename Tc>
float SpatialSampleRule<Trng, Tc>::getAcceptRation(const int &i, const float &scaling) const
{ return (float)(m_maxNumSelected - m_numSelected) / (float)(m_dataSize - i) * scaling; }

template<typename Trng, typename Tc> 
bool SpatialSampleRule<Trng, Tc>::rejectByPosition(const Vector3F &pos)
{
	const int k = PRuleTyp::computeKey((const float *)&pos);
    Vector3F *inCell = m_cellSelPosMap.find(k);
    if(inCell) return true;
    
    if(k != m_touchedCells[13]) {
        PRuleTyp::getLevel27Cells(m_touchedCells, k, 10);
    }
    
    for(int j = 0;j < 27;++j) {
        if(j==13) continue;
        inCell = m_cellSelPosMap.find(m_touchedCells[j]);
        if(!inCell) continue;
        
        if(pos.distanceTo(*inCell) < m_minDistance) return true;
    }
	
	return false;
}

template<typename Trng, typename Tc> 
void SpatialSampleRule<Trng, Tc>::select(const Vector3F &pos)
{
	const int k = PRuleTyp::computeKey((const float *)&pos);
    m_cellSelPosMap.insert(k, pos);
	m_numSelected++;
}
	
}

#endif
