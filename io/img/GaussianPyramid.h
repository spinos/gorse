/*
 *  GaussianPyramid.h
 *  aloe img
 *
 *  multi scale presentation of input signal as level 0  
 *  apply low pass filter and scale down on each level
 *  T pixel type
 *  M max level [0, M]
 *	reference 
 *  http://homepages.inf.ed.ac.uk/rbf/HIPR2/gsmooth.htm
 *  http://songho.ca/dsp/convolution/convolution.html
 *
 *  Created by jian zhang on 3/14/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef APH_IMG_GAUSSIAN_PYRMAID_H
#define APH_IMG_GAUSSIAN_PYRMAID_H

#include <math/Array2.h>
#include "SampleState.h"

namespace alo {

template<typename T, int M>
class GaussianPyramid {
	
	Array2<T> m_stage[M+1];
	int m_dim[M+1][2];
	
	typedef Array2<T> SignalSliceTyp;

public:

	GaussianPyramid();
	virtual ~GaussianPyramid();

	template<typename Tg>
	void create(const Tg& inputSignal, SampleState& state);
	
	const SignalSliceTyp& levelSignal(int level) const;
	
	void sample(float* dst, SampleState& state) const;
	
	void verbose() const;
	
protected:

	void applyBlurFilter(SignalSliceTyp & dst, const SignalSliceTyp & src, SampleState& state);
	void blurVertical(SignalSliceTyp * dstSlice, const SignalSliceTyp & srcSlice,
							const int & m, const int & n, bool toWrap);
	void blurHorizontal(SignalSliceTyp * dstSlice, const SignalSliceTyp & srcSlice,
							const int & m, const int & n, bool toWrap);
	T blurredVertical(const SignalSliceTyp & slice,
					int iRow, int jCol, bool toWrap) const;
	T blurredHorizontal(const SignalSliceTyp & slice,
					int iRow, int jCol, bool toWrap) const;
					
	void scaleDown(SignalSliceTyp & dst, const SignalSliceTyp & src);

private:

/// pixel size at each level
	static const float SampleFilterSize[7];
/// blur filter separated convolution
	static const float BlurKernel[5];
	
};

template<typename T, int M>
const float GaussianPyramid<T, M>::SampleFilterSize[7] = {1.f, 2.f, 4.f, 8.f, 16.f, 32.f, 64.f};

/// (1 4 6.4 4 1) / 16.4
template<typename T, int M>
const float GaussianPyramid<T, M>::BlurKernel[5] = {0.06097560975609757f, 0.24390243902439027f, 0.39024390243902446f, 0.24390243902439027f, 0.06097560975609757f};

template<typename T, int M>
GaussianPyramid<T, M>::GaussianPyramid()
{}

template<typename T, int M>
GaussianPyramid<T, M>::~GaussianPyramid()
{}

template<typename T, int M>
template<typename Tg>
void GaussianPyramid<T, M>::create(const Tg& inputSignal, SampleState& state)
{
	const int& nu = inputSignal.imageWidth();
	const int& nv = inputSignal.imageHeight();
	
	SignalSliceTyp& l0sig = m_stage[0];
	l0sig.create(nv, nu);
	m_dim[0][0] = m_stage[0].numCols();
	m_dim[0][1] = m_stage[0].numRows();
	
	for(int i = 0;i<nu;++i) {
		state.u()[0] = i;
		
		T* col = l0sig.column(i);
		
		for(int j = 0;j<nv;++j) {
			state.v()[0] = j;
			
			inputSignal.sample((float* )&col[j], state);
		}
	}
	
	SignalSliceTyp tmp;

	for(int l = 1;l <= M;++l) {
		applyBlurFilter(tmp, m_stage[l-1], state);
		scaleDown(m_stage[l], tmp);
		m_dim[l][0] = m_stage[l].numCols();
		m_dim[l][1] = m_stage[l].numRows();
	}
}

template<typename T, int M>
void GaussianPyramid<T, M>::applyBlurFilter(SignalSliceTyp & dst, const SignalSliceTyp & src, SampleState& state)
{
	const int & m = src.numRows();
	const int & n = src.numCols();
	dst.create(m, n);
	
	SignalSliceTyp convol;
	convol.create(m, n);
	
	blurVertical(&convol, src, m, n, state.accessV() == SampleState::Wrap);
	blurHorizontal(&dst, convol, m, n, state.accessU() == SampleState::Wrap);
}

template<typename T, int M>
void GaussianPyramid<T, M>::blurVertical(SignalSliceTyp * dstSlice, const SignalSliceTyp & srcSlice,
							const int & m, const int & n, bool toWrap)
{
	for(int j=0;j<n;++j) {
		T * colj = dstSlice->column(j);
		
		for(int i=0;i<m;++i) {
			colj[i] = blurredVertical(srcSlice, i, j, toWrap);
		}
	}
}

template<typename T, int M>
void GaussianPyramid<T, M>::blurHorizontal(SignalSliceTyp * dstSlice, const SignalSliceTyp & srcSlice,
							const int & m, const int & n, bool toWrap)
{
	for(int j=0;j<n;++j) {
		T * colj = dstSlice->column(j);
		
		for(int i=0;i<m;++i) {
			colj[i] = blurredHorizontal(srcSlice, i, j, toWrap);
		}
	}
}

template<typename T, int M>
T GaussianPyramid<T, M>::blurredVertical(const SignalSliceTyp & slice,
					int iRow, int jCol, bool toWrap) const
{
	const int limi = slice.numRows() - 1;
	T sum(0.0);
	int ri;

	const T * colj = slice.column(jCol);
		
	for(int i=0;i<5;++i) {
		ri = iRow + i - 2;
		
		if(ri < 0) {
		    if(toWrap)
		        ri = ri + limi;
		    else 
		        ri = 0;
		}
		if(ri > limi) {
		    if(toWrap)
		        ri = ri - limi;
			else
			    ri = limi;
		}
		
		sum += colj[ri] * BlurKernel[i];
	}
	
	return sum;
	
}

template<typename T, int M>
T GaussianPyramid<T, M>::blurredHorizontal(const SignalSliceTyp & slice,
					int iRow, int jCol, bool toWrap) const
{
	const int limj = slice.numCols() - 1;
	T sum(0.0);
	int rj;

	const T * colj = slice.column(jCol);
		
	for(int i=0;i<5;++i) {
		rj = jCol + i - 2;
		
		if(rj < 0) {
		    if(toWrap)
		        rj = rj + limj;
		    else 
		        rj = 0;
		}
		if(rj > limj) {
		    if(toWrap)
		        rj = rj - limj;
		    else 
		        rj = limj;
		}
		
		sum += slice.column(rj)[iRow] * BlurKernel[i];
	}
	
	return sum;
	
}
	
template<typename T, int M>
void GaussianPyramid<T, M>::scaleDown(SignalSliceTyp & dst, const SignalSliceTyp & src)
{
	const int m = src.numRows() >> 1;
	const int n = src.numCols() >> 1;
	dst.create(m, n);
	
	for(int j=0;j<n;++j) {
		T * colj = dst.column(j);
			
		for(int i=0;i<m;++i) {
			colj[i] = src.column(j<<1)[i<<1];
		}
	}
}

template<typename T, int M>
const Array2<T> & GaussianPyramid<T, M>::levelSignal(int level) const
{ return m_stage[level]; }

template<typename T, int M>
void GaussianPyramid<T, M>::verbose() const
{
	std::cout<<"\n GaussianPyramid n levels "<<M
		<<"\n size0 "<<m_dim[0][0]<<" x "<<m_dim[0][1]
		<<"\n size"<<M<<" "<<m_dim[M][0]<<" x "<<m_dim[M][1];
}

template<typename T, int M>
void GaussianPyramid<T, M>::sample(float* dst, SampleState& state) const
{
	int l = state.levelOfDetail();
	if(l<0) l = 0;
	if(l>M) l = M;
	
	const SignalSliceTyp& slice = levelSignal(l);
	state.computeCoord(slice.numCols(), slice.numRows() );
	
	slice.lookup((char* )state.buf(0), state.u()[0], state.v()[0]);
	if(state.filter() == SampleState::Box) {
		slice.lookup((char* )state.buf(1), state.u()[1], state.v()[0]);
		slice.lookup((char* )state.buf(2), state.u()[0], state.v()[1]);
		slice.lookup((char* )state.buf(3), state.u()[1], state.v()[1]);
		state.computeValue(dst);
	}
	state.setValue(dst);

}

}
#endif