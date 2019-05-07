#include "Binning.h"

namespace alo {

Binning::Binning() : m_numSplits(0)
{}

Binning::~Binning()
{}

void Binning::setEmpty()
{ m_numSplits = 0; }
	
void Binning::create(int n)
{
	m_numSplits = n;
	for(int i=0;i<n+1;++i) {
		m_objectInBin[i] = 0;
		m_aabbInBin[i].reset();
	}
}

void Binning::setSplitPos(float x, int i)
{ m_splitPos[i] = x; }

void Binning::count(float x, const BoundingBox &box)
{
	int i = getBin(x);
	m_objectInBin[i]++;
	m_aabbInBin[i].expandBy(box);
}

int Binning::getBin(float x) const
{
	if(x < m_splitPos[0]) return 0;
	if(x >= m_splitPos[m_numSplits-1]) return m_numSplits;

	int lft = 0;
	int rgt = m_numSplits;
	int mid = (lft + rgt)/2;
	float fm;
	while(rgt > lft+1) {
		fm = m_splitPos[mid];
		if(x < fm) rgt = mid;
		else lft = mid;
		mid = (lft + rgt)/2;
	}
	return rgt;
}

void Binning::scan()
{
	m_countLeft[0] = m_objectInBin[0];
	m_aabbLeft[0] = m_aabbInBin[0];
	for(int i=1;i<m_numSplits;++i) {
		m_countLeft[i] = m_objectInBin[i] + m_countLeft[i-1];
		m_aabbLeft[i] = m_aabbLeft[i-1];
		m_aabbLeft[i].expandBy(m_aabbInBin[i]);
	}

	m_countRight[m_numSplits-1] = m_objectInBin[m_numSplits];
	m_aabbRight[m_numSplits-1] = m_aabbInBin[m_numSplits];
	for(int i=m_numSplits-2;i>=0;--i) {
		m_countRight[i] = m_objectInBin[i+1] + m_countRight[i+1];
		m_aabbRight[i] = m_aabbRight[i+1];
		m_aabbRight[i].expandBy(m_aabbInBin[i+1]);
	}
}

bool Binning::isEmpty() const
{ return m_numSplits < 1; }

const int &Binning::numSplits() const
{ return m_numSplits; }

const float &Binning::splitPos(int i) const
{ return m_splitPos[i]; }

const int &Binning::countInBin(int i) const
{ return m_objectInBin[i]; }

const int &Binning::countLeft(int i) const
{ return m_countLeft[i]; }

const int &Binning::countRight(int i) const
{ return m_countRight[i]; }

const BoundingBox &Binning::aabbLeft(int i) const
{ return m_aabbLeft[i]; }

const BoundingBox &Binning::aabbRight(int i) const
{ return m_aabbRight[i]; }

std::ostream& operator<<(std::ostream &output, const Binning & p) 
{
	output << "\n [";
    for(int i=0; i<=p.numSplits();++i) {
    	output << " " << p.countInBin(i);
    }
    output << "] \n   [";
    for(int i=0; i<p.numSplits();++i) {
    	output << " " << p.countLeft(i);
    }
    output << "] \n   [";
    for(int i=0; i<p.numSplits();++i) {
    	output << " " << p.countRight(i);
    }
    output << "]";
    return output;
}

}
