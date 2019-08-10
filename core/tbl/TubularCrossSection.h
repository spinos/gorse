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

class TubularCrossSection {

	Polygon2D *m_lass;

public:

	TubularCrossSection();
	virtual ~TubularCrossSection();

/// nu # vertices
/// a, b semi axis length of ellipse
	void create(int nu, float a, float b);

protected:

private:

};
	
}

#endif
