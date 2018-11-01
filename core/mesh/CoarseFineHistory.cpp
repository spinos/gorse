#include "CoarseFineHistory.h"

namespace alo {

CoarseFineHistory::CoarseFineHistory()
{}

CoarseFineHistory::~CoarseFineHistory()
{}

void CoarseFineHistory::initialize(int nf)
{
	static const int unn = 1<<30;

	m_length = nf;
		
	m_history.createBuffer(m_length);
	for(int i=0;i<m_length;++i)
		m_history[i] = unn;
}

void CoarseFineHistory::setVCF(const int &vbegin, const int &ncoarse, const int &nfine)
{
	m_vbegin = vbegin; 
    m_ncoarse = ncoarse;
    m_nfine = nfine;
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
    m_length += n;
}

void CoarseFineHistory::setFRange(int x, int y) 
{
	m_fend = x;
	m_fbegin = y;
}

const int &CoarseFineHistory::vbegin() const
{ return m_vbegin; }

int &CoarseFineHistory::vbegin()
{ return m_vbegin; }

const int &CoarseFineHistory::vend() const
{ return m_vend; }

int &CoarseFineHistory::vend()
{ return m_vend; }

const int &CoarseFineHistory::nfine() const
{ return m_nfine; }

const int &CoarseFineHistory::ncoarse() const
{ return m_ncoarse; }

const int &CoarseFineHistory::length() const
{ return m_length; }

const int &CoarseFineHistory::fbegin() const
{ return m_fbegin; }

const int &CoarseFineHistory::fend() const
{ return m_fend; }

const int *CoarseFineHistory::c_value() const
{ return m_history.c_data(); }

int CoarseFineHistory::coarseMax() const
{ return m_fbegin - 1; }

int CoarseFineHistory::fineMax() const
{ return m_fend - 1; }

void CoarseFineHistory::printDetail() const
{
	static const int unn = 1<<29;

	std::cout << "\n coarse [";
    for(int i=0;i<m_fbegin;++i) {
        const int & vi = m_history[i];
	    if(i % 100 == 0) std::cout << "\n";
		if(i % 10 == 0) std::cout << " " << i << ": ";
	    if(vi > unn)
		    std::cout << "." ;
	    else 
	        std::cout << " " << vi;
	}
	std::cout << "]\n\n fine [";

	for(int i=m_fbegin;i<m_fend;++i) {
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
