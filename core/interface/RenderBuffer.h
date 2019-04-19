#ifndef ALO_RENDER_BUFFER_H
#define ALO_RENDER_BUFFER_H

#include "BufferParameters.h"
#include "prng.h"

namespace alo {

class ImageFragment;
class RenderContext;
class BufferBlock;

class RenderBuffer {

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
	
protected:

private:
	
	ImageFragment* fragment();
	float& pixelDepthBuffer(const int& i);

};

}

#endif
