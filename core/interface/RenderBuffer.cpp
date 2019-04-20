/*
 *  RenderBuffer.h
 *  gorse
 *
 *  2019/4/20
 */

#include "RenderBuffer.h"
#include "BufferBlock.h"
#include "RenderContext.h"
#include "ImageFragment.h"
#include "PixelProjector.h"
#include "IntersectResult.h"
#include <img/SampleState.h>

namespace alo {

RenderBuffer::RenderBuffer()
{
	m_primaryIntersection = new IntersectResult;
	m_evironmentLatlongState = new SampleState;
	m_evironmentLatlongState->setAccess(SampleState::Wrap, SampleState::Clamp);
	m_evironmentLatlongState->setFilter(SampleState::Box);
	m_evironmentLatlongState->setChannel(SampleState::RGB);
	m_fragment = new ImageFragment;
}

RenderBuffer::~RenderBuffer()
{
	delete m_fragment;
	delete m_prng;
	delete m_primaryIntersection;
	delete m_evironmentLatlongState;
}

void RenderBuffer::createRng(int s)
{ m_prng = new PrngTyp(s); }

PrngTyp* RenderBuffer::prng()
{ return m_prng; }

void RenderBuffer::reproject(RenderContext& context, BufferBlock& blk) const
{
    blk.progressAge();
	PixelProjector* prjr = context.projector();
	float res = prjr->reproject(blk.fragment(), blk.age(), blk.inverseAge(), *m_fragment);
	blk.setResidual(res);
}

void RenderBuffer::setFragmentColor(const float* x, const int& i)
{ m_fragment->setColor(x, i); }

ImageFragment* RenderBuffer::fragment()
{ return m_fragment; }

float& RenderBuffer::pixelDepthBuffer(const int& i)
{ return m_depth[i]; }

int RenderBuffer::fragmentNumPixels() const
{ return m_fragment->fragmentSize(); }

IntersectResult *RenderBuffer::primaryIntersction()
{ return m_primaryIntersection; }

SampleState *RenderBuffer::evironmentLatlongSampleState()
{ return m_evironmentLatlongState; }

}
