/*
 *  RenderBuffer.h
 *  gorse
 *
 *  access to random number generator, pixel color and depth, 
 *  intersection result, sample states
 *
 *  2019/4/20
 */

#ifndef ALO_RENDER_BUFFER_H
#define ALO_RENDER_BUFFER_H

#include "BufferParameters.h"
#include "prng.h"

namespace alo {

class ImageFragment;
class RenderContext;
class BufferBlock;
class SampleState;
class IntersectResult;

class RenderBuffer {

	IntersectResult *m_primaryIntersection;
	SampleState *m_evironmentLatlongState;
	PrngTyp* m_prng;
/// pixel color
	ImageFragment* m_fragment;
/// and depth
	float m_depth[MAX_BUFFER_BLOCK_SIZE];

public:

	RenderBuffer();
	virtual ~RenderBuffer();

	void createRng(int s);

	PrngTyp* prng();

/// i-th pixel	
	void setFragmentColor(const float* x, const int& i);
	void reproject(RenderContext& context, BufferBlock& blk) const;

	int fragmentNumPixels() const;

	IntersectResult *primaryIntersction();
	SampleState *evironmentLatlongSampleState();
	
protected:

private:
	
	ImageFragment* fragment();
	float& pixelDepthBuffer(const int& i);

};

}

#endif
