#include "RenderBuffer.h"
#include "BufferBlock.h"
#include "RenderContext.h"
#include "ImageFragment.h"
#include "PixelProjector.h"
#include <rng/Uniform.h>
#include <rng/Lehmer.h>

namespace alo {

RenderBuffer::RenderBuffer()
{
	m_fragment = new ImageFragment;
}

RenderBuffer::~RenderBuffer()
{
	delete m_fragment;
	delete m_prng;
}

void RenderBuffer::createRng(int s)
{ m_prng = new PrngTyp(s); }

PrngTyp* RenderBuffer::prng()
{ return m_prng; }

void RenderBuffer::reproject(RenderContext& context, BufferBlock& blk) const
{
    blk.progressAge();
	PixelProjector* prjr = context.projector();
	float res = prjr->reproject(blk, *m_fragment);
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

}
