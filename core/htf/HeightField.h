/*
 * HeightField.h
 * aloe
 *
 * uniform 2d array of height values
 *
 * 2019/8/8
 */

#ifndef ALO_HTF_HEIGHT_FIELD_H
#define ALO_HTF_HEIGHT_FIELD_H

#include <math/Array2.h>

namespace alo {

class Vector3F;
class Polygon2D;

template<typename T>
class Bilinear;

class HeightField : public Array2<float> {

	float m_gridSize;
	float m_heightScale;
	float m_origin[3];

public:

	HeightField();
	virtual ~HeightField();

	void setGridSize(float x);
	void setHeightScale(float x);
	void setOrigin(const float *x);
/// inside region
	void setValue(const Bilinear<float> &fval, const Polygon2D &region);

	void smooth();

	void getResolution(int &h, int &v) const;
/// i-th column, j-th row
	Vector3F vertex(const int &i, const int &j) const;

protected:

private:

};

}

#endif
