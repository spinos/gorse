/*
 *  TubularCrossSection.h
 *  gorse
 *
 *  2019/8/10
 */

#ifndef ALO_TUBULAR_CROSS_SECTION_H
#define ALO_TUBULAR_CROSS_SECTION_H

namespace alo {

class Polygon2D;
class Vector3F;

class TubularCrossSection {

	Polygon2D *m_lass;

public:

	TubularCrossSection();
	virtual ~TubularCrossSection();

/// nu # vertices
/// a, b semi axis length of ellipse
	void create(int nu, float a, float b);

	int numVertices() const;
	void getVertex(float *q, int i) const;

protected:

private:

};
	
}

#endif
