#ifndef ALO_VECTOR_N_H
#define ALO_VECTOR_N_H

#include <iostream>
#include <sstream>
#include <string.h>

namespace alo {

template<typename T>
struct VectorN {

	VectorN() {
		_ndim = 0;
		_data = 0;
	}
	
	VectorN(const int & n) {
		_ndim = n;
		_data = new T[n];
	}
	
	~VectorN() {
		if(_data) delete[] _data;
	}
	
	const int & N() const
	{ return _ndim; }
	
	const T * v() const {
		return _data;
	}
	
	T * v() {
		return _data;
	}
	
	void create(const int & n)
	{
		if(_ndim < n) {
			if(_ndim > 0)
				delete[] _data;
			_data = new T[n];
		}
		_ndim = n;
	}
	
	void create(const T * x, const int & n)
	{
		create(n);
		memcpy(_data, x, sizeof(T) * n);
	}
	
	int periodic(const int & i) const {
		if(i<0)
			return _ndim +i;
		if(i>_ndim-1)
			return i-_ndim;
		return i;
	}
	
	void copy(const T * v, const int & n, const int & p=0) {
		create(n);
		if(p==0) {
			memcpy(_data, v, sizeof(T) * n);
			return;
		}
		
		const int q = p>0 ? p : -p;

		if(p<0) {
			memcpy(_data, &v[q], sizeof(T) * (n-q) );
			memcpy(&_data[n-q], v, sizeof(T) * q );
		}
		else {
			memcpy(&_data[q], v, sizeof(T) * (n-q) );
			memcpy(_data, &v[n-q], sizeof(T) * q );
		}
	}
	
/// copy with shift phase
	void copy(const VectorN<T> & another, const int & p = 0) {
		copy(another.v(), another.N(), p);
	}
	
	void setZero(const int & n) {
		create(n);
		memset(_data, 0, sizeof(T) * _ndim);
	}
	
	void setZero() {
		memset(_data, 0, sizeof(T) * _ndim);
	}
	
	void operator=(const VectorN<T> & another) {
		copy(another);
	}
	
	const T& operator[](const int & i) const {
		return _data[i];
	}
	
	T& operator[](const int & i) {
		return _data[i];
	}
	
	T* at(const int & i) const {
		return &_data[i];
	}
	
	VectorN<T> operator+(const VectorN<T> & another) const {
		VectorN<T> r(_ndim);
		for(int i = 0; i < _ndim; i++) *r.at(i) = _data[i] + another[i];
		return r;
	}
	
	VectorN<T> operator-(const VectorN<T> & another) const {
		VectorN<T> r(_ndim);
		for(int i = 0; i < _ndim; i++) *r.at(i) = _data[i] - another[i];
		return r;
	}
	
	VectorN<T> operator*(const T & scale) const {
		VectorN<T> r(_ndim);
		for(int i = 0; i < _ndim; i++) *r.at(i) = _data[i] * scale;
		return r;
	}
	
	void operator*=(const T & scale) {
		for(int i = 0; i < _ndim; i++) 
			_data[i] *= scale;
	}
	
	void operator+=(const VectorN<T> & another) {
		for(int i = 0; i < _ndim; i++) 
			_data[i] += another[i];
	}
	
	T multiplyTranspose() const {
		T r;
		for(int i = 0; i < _ndim; i++) r += _data[i] * _data[i];
		return r;
	}
	
	void maxAbsError(T & err, const VectorN & another) const {
		int i = 0;
		for(;i<_ndim;++i) {
			T d = _data[i] - another.v()[i];
			if(d < 0) d = -d;
			if(err < d)
				err = d;
		}
	}
	
/// decrease sampling rate by integer p with phase offset
/// X[N] input signal
	void downsample(const T * x, const int & n, 
					const int & p, const int & phase=0) {
	
		int i=0, j=0;
		for(;i<n;++i) {
			if(i== j*p + phase) {
				j++;
			}
		}
		
		create(j);
		
		i=j=0;
		for(;i<n;++i) {
			if(i== j*p + phase) {
				v()[j++]=x[i];
			}
		}
	}
	
/// P phase of shift |P| < N
/// delay the signal when P > 0	
	void circshift(const int & p) {
		if(p==0) 
			return;
		
		VectorN<float> b;
		b.copy(*this);
		copy(b, p);
		
	}
	
/// find i where v[i] < k <= v[i+1]
	int lowerBound(const float& k) const;
/// r [0,1]
	const T& sample(const T& r) const;
	
	std::string info() const {
		std::stringstream sst;
		sst.str("");
		sst<<"(";
		for(int i = 0; i < _ndim - 1; i++) sst<<_data[i]<<", ";
		sst<<_data[_ndim - 1]<<")";
		return sst.str();
	}
	
	int _ndim;
	T * _data;
};

template<typename T>
int VectorN<T>::lowerBound(const float& k) const
{
	if(_data[0] >= k) return -1;
	if(_data[_ndim-1] < k) return _ndim-1;
	int a = 0;
	int b = _ndim-1;
	while(b > a + 1) {
		if(_data[b] == k) return b - 1;
		int c = (a + b) / 2;
		if(_data[c] == k) return c - 1;
		if(_data[c] < k) {
			a = c;
		} else {
			b = c;
		}
	}
	return a;
}

template<typename T>
const T& VectorN<T>::sample(const T& r) const
{
	int i = r * _ndim;
	i = i % _ndim;
	return _data[i];
}

}
#endif        //  #ifndef APHID_TYPES_H
