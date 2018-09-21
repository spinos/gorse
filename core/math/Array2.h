#ifndef ALO_ARRAY_2_H
#define ALO_ARRAY_2_H

#include <iostream>
#include <sstream>
#include <string.h>

namespace alo {

/// transform and out-of-bound return value
template<typename T>
struct Array2SampleProfile {

	float _translate[2];
	float _scale[2];
	T _defaultValue;
	
};

/// m-by-n array
/// column major
/// 0   m   ... (n-1)m
/// 1   m+1     (n-1)m+1
/// .   .       .
/// .   .       .
/// .   .       .
/// m-1 2m-1... nm-1
/// m number of rows
/// n number of columns
template<typename T>
struct Array2 {

	T * m_data;
	int m_M, m_N;
	
	Array2() {
		m_M = m_N = 0;
		m_data = NULL;
	}
	
	Array2(const Array2<T> & another) {
		m_M = m_N = 0;
		m_data = NULL;
		copy(another);
	}
	
	~Array2() {
		if(m_M>0) delete[] m_data;
	}
	
	void create(const int & m, const int & n) {
		if(m_M * m_N < m*n) {
			if(m_M) delete[] m_data;
			m_data = new T[m*n];
		}
		
		m_M = m;
		m_N = n;
	}
	
	const int & numRows() const {
		return m_M;
	}
	
	const int & numCols() const {
		return m_N;
	}
	
	const T * v() const {
		return m_data;
	}
	
	T * v() {
		return m_data;
	}

	const T * column(const int & i) const {
		return &m_data[i*m_M];
	}
	
	T * column(const int & i) {
		return &m_data[i*m_M];
	}
	
/// u column v row
	int iuv(const int & u, const int & v) const {
		return u * m_M + v;
	}
	
	void operator=(const Array2<T> & another) {
		copy(another);
	}
	
	void copy(const Array2<T> & another) {
		create(another.numRows(), another.numCols() );
		memcpy(m_data, another.v(), m_M*m_N*sizeof(T) );
	}
	
	void setZero() {
		memset (m_data, 0, m_M*m_N*sizeof(T) );
	}
	
	void copyColumn(const int & i, const T * b) {
		memcpy(column(i), b, m_M *sizeof(T) );
	}
	
	void transpose() {
		Array2 old(*this);
		
		int s = m_M;
		m_M = m_N;
		m_N = s;
		
		const T * src = old.v();
		
		int i, j;
		for(j = 0;j<m_N;++j) {
			
			T * dst = column(j);
			for(i=0;i<m_M;++i) {
				dst[i] = src[i * m_N + j];
			}
		}
	}
	
	void maxAbsError(T & err, const Array2 & another) const {
		const int mn = m_M * m_N;
		int i = 0;
		for(;i<mn;++i) {
			T d = m_data[i] - another.v()[i];
			if(d < 0) d = -d;
			if(err < d)
				err = d;
		}
	}
	
	int maxDim() const {
		return m_M > m_N ? m_M : m_N;
	}
	
	Array2<T> operator+(const Array2<T> & another) const {
		Array2<T> r;
		r.copy(*this);
		r += another;
		return r;
	}
	
	Array2<T> operator-(const Array2<T> & another) const {
		Array2<T> r;
		r.copy(*this);
		r -= another;
		return r;
	}
	
	void operator+=(const Array2<T> & another) {
		const int mn = m_M * m_N;
		for(int i = 0; i < mn;++i) 
			m_data[i] += another.v()[i];
	}
	
	void operator-=(const Array2<T> & another) {
		const int mn = m_M * m_N;
		for(int i = 0; i < mn;++i) 
			m_data[i] -= another.v()[i];
	}
	
	void operator*=(const T & s) {
		const int mn = m_M * m_N;
		for(int i = 0; i < mn; i++) 
			m_data[i] *= s;
	}
	
	friend std::ostream& operator<<(std::ostream &output, const Array2 & p) {
        output << p.str();
        return output;
    }
	
	const std::string str() const {
		std::stringstream sst;
		sst.str("");
		
		for(int j=0;j<m_M;++j) {
			sst<<"\n|";
			for(int i=0;i<m_N;++i) {
				sst<<m_data[iuv(i,j)];
				if(i<m_N-1)
					sst<<", ";
			}
			sst<<"|";
		}
		return sst.str();
	}
	
/// get a part of 
	void sub(Array2 & d,
			const int & m0, const int & n0) const {
		
		const int & ms = d.numRows();
		const int & ns = d.numCols();
		
		for(int i=0;i<ns;++i) {
			memcpy(d.column(i), &column(n0+i)[m0], sizeof(T) * ms);
		}
	}
	
	void sample(const Array2<T> & another, 
		const Array2SampleProfile<T> * prof);
	
/// map value by coord u in row v in column
/// return false if coord is out-of-bound
	bool getValue(T & dst, const float & u, const float & v) const;
	
	void convoluteVertical(const Array2<T> & src,
                const T * kern3tap);
	void convoluteHorizontal(const Array2<T> & src,
                const T * kern3tap);
/// vmin vmax already have values				
	void getMinMax(T & vmin, T & vmax) const;
/// set all values
	void set(const T & x);
	
/// find j where v[j] < k <= v[j+1] in i-th column
	int lowerBound(const float& k, const int& i) const;
	
	void lookup(char* dst, const int& iu, const int& iv) const;
	
};

template<typename T>
bool Array2<T>::getValue(T & dst, const float & u, const float & v) const
{
	if(u < 0.f || u > 1.f) {
		return false;
	}
	if(v < 0.f || v > 1.f) {
		return false;
	}
	
	float fu = u * (float)m_N - .5f;
	float fv = v * (float)m_M - .5f;
	int u0 = fu;
	int v0 = fv;
	int u1 = u0 + 1;
	int v1 = v0 + 1;
	
	if(u0 < 0) {
		u0 = 0;
	}
	if(v0 < 0) {
		v0 = 0;
	}
	if(u1 > m_N - 1) {
		u1 = m_N - 1;
	}
	if(v1 > m_M - 1) {
		v1 = m_M - 1;
	}
	
	fu -= u0;
	fv -= v0;
	if(fu < 0.f) {
		fu = 0.f;
	}
	if(fv < 0.f) {
		fv = 0.f;
	}
	
	T box[4];
	box[0] = m_data[u0 * m_M + v0];
	box[1] = m_data[u0 * m_M + v1];
	box[2] = m_data[u1 * m_M + v0];
	box[3] = m_data[u1 * m_M + v1];
	
	box[0] += fv * (box[1] - box[0]);
	box[2] += fv * (box[3] - box[2]);
		
	dst = box[0] + fu * (box[2] - box[0]);
	
	return true;
}

template<typename T>
void Array2<T>::sample(const Array2<T> & another, 
		const Array2SampleProfile<T> * prof)
{
	const float di = 1.f / (float)(m_M);
	const float dj = 1.f / (float)(m_N);
	const float hdi = di * .5f;
	const float hdj = dj * .5f;
	
	float u, v;
	
	for(int j=0;j<m_N;++j) {
		u = (dj * j + hdj) * prof->_scale[0] + prof->_translate[0];
		
		T * dc = column(j);
		for(int i=0;i<m_M;++i) {
		
			v = (di * i + hdi) * prof->_scale[1] + prof->_translate[1];
			
			if(!another.getValue(dc[i], u, v) ) {
				dc[i] = prof->_defaultValue;
			}
		}
	}
}

template<typename T>
void Array2<T>::convoluteVertical(const Array2<T> & src,
                const T * kern3tap)
{
	const int & m = numRows();
	const int & n = numCols();
	const int limi = m - 1;
	int ri;
	for(int j=0;j<n;++j) {
		
		const T * srcColj = src.column(j);
		T * colj = column(j);
		
		for(int i=0;i<m;++i) {
		
			T & vi = colj[i];
			
			vi = (T)0.0;
			
			ri = i - 1;
			if(ri < 0) {
				ri = 0;
			}
			
			vi += srcColj[ri] * kern3tap[0];
			
			vi += srcColj[i] * kern3tap[1];
			
			ri = i + 1;
			if(ri > limi) {
				ri = limi;
			}
			
			vi += srcColj[ri] * kern3tap[2];
			
		}
	}
}

template<typename T>
void Array2<T>::convoluteHorizontal(const Array2<T> & src,
                const T * kern3tap)
{
	const int & m = numRows();
	const int & n = numCols();
	const int limj = n - 1;
	int j0, j1;
	for(int j=0;j<n;++j) {
		
		T * colj = column(j);
		
		const T * srcColj = src.column(j);
		
		j0 = j - 1;
		if(j0 < 0) {
			j0 = 0;
		}
		
		const T * srcColj0 = src.column(j0);
		
		j1 = j + 1;
		if(j1 > limj) {
			j1 = limj;
		}
		
		const T * srcColj1 = src.column(j1);
		
		for(int i=0;i<m;++i) {
		
			T & vi = colj[i];
			
			vi = (T)0.0;
			
			vi += srcColj0[i] * kern3tap[0];
			vi += srcColj[i] * kern3tap[1];
			vi += srcColj1[i] * kern3tap[2];
			
		}
	}
	
}

template<typename T>
void Array2<T>::getMinMax(T & vmin, T & vmax) const
{
	const int mn = numCols() * numRows();
	for(int i=0;i<mn;++i) {
		const T & vi = m_data[i];
		if(vmin > vi) {
			vmin = vi;
		}
		if(vmax < vi) {
			vmax = vi;
		}
	}
}

template<typename T>
void Array2<T>::set(const T & x)
{
	const int mn = numCols() * numRows();
	for(int i=0;i<mn;++i) {
		m_data[i] = x;
	}
}

template<typename T>
int Array2<T>::lowerBound(const float& k, const int& i) const
{
	const T* d = column(i);
	const int& nrow = numRows();
	
	if(d[0] >= k) return -1;
	if(d[nrow-1] < k) return nrow-1;
	int a = 0;
	int b = nrow-1;
	while(b > a + 1) {
		if(d[b] == k) return b - 1;
		int c = (a + b) / 2;
		if(d[c] == k) return c - 1;
		if(d[c] < k) {
			a = c;
		} else {
			b = c;
		}
	}
	return a;
}

template<typename T>
void Array2<T>::lookup(char* dst, const int& iu, const int& iv) const
{ column(iu)[iv].extract(dst); }

}
#endif        //  #ifndef APHID_TYPES_H
