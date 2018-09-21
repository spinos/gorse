/*
 *  DrawArrow.h
 *  
 *
 *  Created by jian zhang on 1/1/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef APH_OGL_DRAW_ARROW_H
#define APH_OGL_DRAW_ARROW_H

namespace alo {

class Matrix44F;
class Matrix33F;

class DrawArrow {

public:
	DrawArrow();
	virtual ~DrawArrow();
	
	void begin();
	void end();
/// between begin and end
	void drawBoneArrowAt(const Matrix44F * mat);
	void drawArrowAt(const Matrix44F * mat);
	void drawCoordinateAt(const Matrix44F * mat);
	void drawCoordinateTandem(const Matrix44F * mat,
						const Matrix33F * mat1);
	void drawFlatArrowAt(const Matrix44F * mat);
	void drawFlatArrowTandem(const Matrix44F * mat,
						const Matrix33F * mat1);
	
protected:
    void drawXArrow() const;
    void drawYArrow() const;
    void drawZArrow() const;
    
private:
	void drawFlatArraw();
	void drawCoordinate();
	
};

}
#endif