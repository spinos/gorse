/*
 *  ShapeDrawer.h
 *  aloe
 *
 *  Created by zhang on 18-2-16.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SHAPE_DRAWER_H
#define ALO_SHAPE_DRAWER_H

namespace alo {

class ShapeDrawer {
public:
    
    static void BeginPoints();
    static void BeginLines();
    static void End();
	static void BeginVertexState();
	static void EndVertexState();
	static void Vertex(const float* p);
	static void Line(const float* p, const float* q);
/// coord is (center, half_span)
    static void BoundingBox(const float* coord);
/// (x0,y0,z0,x1,y1,z1)
    static void BoundingBoxMinMax(const float* coord);
    static void Coordsys(const float& scale);
    static void CoordsysArrowed(const float& scale);
/// from a to b
	static void Arrow(const float* a, const float* b);
/// r is (origin, direction, t_min, t_max)
    static void Ray(const float* r);
    static void RayArrowed(const float* r);
/// n indices k vertices stride
    static void TriangleVertexElement(const float* p, const int* ind,
                    int n, int k);
	static void PointVertexArray(const float* p, int n, int k);
/// n vertices k stride
    static void TriangleVertexArray(const float* p, int n, int k);
/// (d+1)^3 value points
	static void ColorGrid(const float* u, const float* c, int d);
/// color
	static void SetColor(float r, float g, float b);
	static void LightRed();
    static void LightGreen();
	static void LightBlue();
    static void LightYellow();
    static void DarkRed();
    static void DarkGreen();
    static void DarkBlue();
	
	static void BeginSpace(const float* mat);
	static void EndSpace();
    
private:

/// (red, green, blue, black)
    static const float UnitColor[4][4];
/// 6 triangles
    static const float PyramidV[18][4];
    static const float PyramidN[18][4];
/// 12 box edges
    static const float CenterBoxEdgeV[24][3];
    static const int MinMaxBoxEdgeV[24][3];

};

}

#endif