#include "CoarseFineHistory.h"

namespace alo {

CoarseFineHistory::CoarseFineHistory()
{}

CoarseFineHistory::~CoarseFineHistory()
{}

void CoarseFineHistory::purgeHistory()
{ m_history.purgeBuffer(); }

void CoarseFineHistory::createHistory(int nf)
{ m_history.resetBuffer(nf); }

void CoarseFineHistory::setDesc(const CoarseFineHistoryDesc &x)
{ m_desc = x; }

void CoarseFineHistory::initialize(int nf)
{
	static const int unn = 1<<30;

	m_desc._length = nf;
		
	m_history.createBuffer(m_desc._length);
	for(int i=0;i<m_desc._length;++i)
		m_history[i] = unn;
}

void CoarseFineHistory::setVCF(const int &vbegin, const int &ncoarse, const int &nfine)
{
	m_desc._vbegin = vbegin; 
    m_desc._ncoarse = ncoarse;
    m_desc._nfine = nfine;
}

void CoarseFineHistory::setNv(const int &x, int location)
{ m_history[location] = x; }

void CoarseFineHistory::swap(int a, int b)
{
	m_history.swap(a, b, 1);
}

void CoarseFineHistory::insert(int x, int n, int location)
{
	int *b = new int[n];
    for(int i=0;i<n;++i)
        b[i] = x;
    
    m_history.insert(b, n, location);
    delete[] b;
    m_desc._length += n;
}

void CoarseFineHistory::setFRange(int x, int y) 
{
	m_desc._fend = x;
	m_desc._fbegin = y;
}

const int &CoarseFineHistory::vbegin() const
{ return m_desc._vbegin; }

int &CoarseFineHistory::vbegin()
{ return m_desc._vbegin; }

const int &CoarseFineHistory::vend() const
{ return m_desc._vend; }

int &CoarseFineHistory::vend()
{ return m_desc._vend; }

const int &CoarseFineHistory::nfine() const
{ return m_desc._nfine; }

const int &CoarseFineHistory::ncoarse() const
{ return m_desc._ncoarse; }

const int &CoarseFineHistory::length() const
{ return m_desc._length; }

const int &CoarseFineHistory::fbegin() const
{ return m_desc._fbegin; }

const int &CoarseFineHistory::fend() const
{ return m_desc._fend; }

const int *CoarseFineHistory::c_value() const
{ return m_history.c_data(); }

int *CoarseFineHistory::value()
{ return m_history.data(); }

int CoarseFineHistory::coarseMax() const
{ return m_desc._fbegin - 1; }

int CoarseFineHistory::fineMax() const
{ return m_desc._fend - 1; }

const CoarseFineHistoryDesc &CoarseFineHistory::desc() const
{ return m_desc; }

void CoarseFineHistory::operator=(const CoarseFineHistory &b)
{
	m_desc = b.desc();
    m_history.createBuffer(m_desc._length);
	memcpy(m_history.data(), b.c_value(), m_desc._length<<2);
}

void CoarseFineHistory::printDetail() const
{
	static const int unn = 1<<29;

	std::cout << "\n coarse [";
    for(int i=0;i<m_desc._fbegin;++i) {
        const int & vi = m_history[i];
	    if(i % 100 == 0) std::cout << "\n";
		if(i % 10 == 0) std::cout << " " << i << ": ";
	    if(vi > unn)
		    std::cout << "." ;
	    else 
	        std::cout << " " << vi;
	}
	std::cout << "]\n\n fine [";

	for(int i=m_desc._fbegin;i<m_desc._fend;++i) {
	    const int & vi = m_history[i];
	    if(i % 100 == 0) std::cout << "\n";
		if(i % 10 == 0) std::cout << " " << i << ": ";
	    if(vi > unn)
		    std::cout << "." ;
	    else 
	        std::cout << " " << vi;
	}
	std::cout << "] ";
}

std::ostream& operator<<(std::ostream &output, const CoarseFineHistory & p)
{
        output << " v [" << p.vbegin() << ": " << p.vend() << "] "
				<< " f [" << p.fbegin() << ": " << p.fend() << "] "
				<< " +coarse " << p.ncoarse ()
				<< " -fine " << p.nfine()
				<< " len " << p.length();
        return output;
}

}
