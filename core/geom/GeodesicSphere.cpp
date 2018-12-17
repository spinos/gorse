/*
 *  GeodesicSphere.h
 *  aloe
 *
 */

#include "GeodesicSphere.h"
#include <math/miscfuncs.h>

namespace alo {
    
GeodesicSphere::GeodesicSphere(int level)
{
    int nvpp = 0;
    for(int i=1;i<level+2;++i)
        nvpp += i;

    int nv = nvpp * 8;
    int nt = level * level * 8;
    createTriangleMesh(nv, nt);
    
    Vector3F * p = positions();
	Int3 * idx = indices();
	
	unsigned currentidx = 0;
	unsigned currentver = 0;
	
	const Vector3F a(0.f, 1.f, 0.f);
	const Vector3F b(-1.f, 0.f, 0.f);
	const Vector3F c(0.f, 0.f, 1.f);
	const Vector3F d(1.f, 0.f, 0.f);
	const Vector3F e(0.f, 0.f, -1.f);
	const Vector3F f(0.f, -1.f, 0.f);
	
	subdivideTriangle(level, currentver, currentidx, p, idx, c, a, b);
	subdivideTriangle(level, currentver, currentidx, p, idx, c, b, f);
	subdivideTriangle(level, currentver, currentidx, p, idx, c, f, d);
	subdivideTriangle(level, currentver, currentidx, p, idx, c, d, a);

    subdivideTriangle(level, currentver, currentidx, p, idx, e, b, a);
    subdivideTriangle(level, currentver, currentidx, p, idx, e, f, b);
    subdivideTriangle(level, currentver, currentidx, p, idx, e, d, f);
    subdivideTriangle(level, currentver, currentidx, p, idx, e, a, d);

    Vector3F *nml = normals();
	for(int i=0;i<nv;++i) {
	     p[i].normalize();
	     nml[i] = p[i];
	}

    Float2 *uvd = addUVSet("map1");

    const Float2 auv(.5f, 1.f);
    const Float2 buv(0.f, .5f);
    const Float2 cuv(.5f, .5f);
    const Float2 duv(1.f, .5f);
    const Float2 e0uv(0.f, 1.f);
    const Float2 e1uv(0.f, 0.f);
    const Float2 e2uv(1.f, 0.f);
    const Float2 e3uv(1.f, 1.f);
    const Float2 fuv(.5f, 0.f);

    int faceVertexIndex = 0;
    subdivideUVTriangle(level, faceVertexIndex, uvd, cuv, auv, buv );
    subdivideUVTriangle(level, faceVertexIndex, uvd, cuv, buv, fuv );
    subdivideUVTriangle(level, faceVertexIndex, uvd, cuv, fuv, duv );
    subdivideUVTriangle(level, faceVertexIndex, uvd, cuv, duv, auv );
    subdivideUVTriangle(level, faceVertexIndex, uvd, e0uv, buv, auv );
    subdivideUVTriangle(level, faceVertexIndex, uvd, e1uv, fuv, buv );
    subdivideUVTriangle(level, faceVertexIndex, uvd, e2uv, duv, fuv );
    subdivideUVTriangle(level, faceVertexIndex, uvd, e3uv, auv, duv );
}

GeodesicSphere::~GeodesicSphere()
{}

void GeodesicSphere::subdivideTriangle(int level, unsigned & currentVertex, unsigned & currentIndex, 
        Vector3F * p, Int3 * idx, 
        const Vector3F & a, const Vector3F & b, const Vector3F & c)
{
    int offset = currentVertex;
    Vector3F delta_ab = (b - a) / (float)level;
    Vector3F delta_bc = (c - b) / (float)level;

    p[currentVertex] = a;           
    currentVertex++; 

    for(int j = 1; j <= level; j++)
    {
        for(int i = 0; i <= j; i++)
        {
            Vector3F row = a + delta_ab * (float)j + delta_bc * (float)i;
            
            p[currentVertex] = row;           
            currentVertex++; 
        }
    }

    int row0 = offset;
    int row1 = 1 + offset;
    for(int j = 0; j < level; j++)
    {
        for(int i = 0; i <= j; i++)
        {
            idx[currentIndex].set( row0 + i ,           
                                    row1 + i,           
                                    row1 + i + 1);           
            currentIndex++;

            if(i<j) {
                idx[currentIndex].set(row0 + i,           
                                        row1 + i + 1,           
                                        row0 + i + 1);           
                currentIndex++;
            }
        }

        row0 = row1;
        row1 += j+2;
    }
}

void GeodesicSphere::subdivideUVTriangle(int level, int & faceVertexIndex,
        Float2 * p, 
        const Float2 & a, const Float2 & b, const Float2 & c)
{
    Float2 delta_ab((b.x - a.x) / (float)level, (b.y - a.y) / (float)level);
    Float2 delta_bc((c.x - b.x) / (float)level, (c.y - b.y) / (float)level);

    for(int j = 0; j < level; j++)
    {
        for(int i = 0; i <= j; i++)
        {
            Float2 v0(a.x + delta_ab.x * (float)j + delta_bc.x * (float)i, 
                        a.y + delta_ab.y * (float)j + delta_bc.y * (float)i);

            Float2 v1(v0.x + delta_ab.x, 
                        v0.y + delta_ab.y);

            Float2 v2(v1.x + delta_bc.x, 
                        v1.y + delta_bc.y);

            Float2 v3(v0.x + delta_bc.x, 
                        v0.y + delta_bc.y);
        
            p[faceVertexIndex] = v0;           
            faceVertexIndex++;

            p[faceVertexIndex] = v1;           
            faceVertexIndex++;

            p[faceVertexIndex] = v2;           
            faceVertexIndex++;

            if(i<j) {
                p[faceVertexIndex] = v0;           
                faceVertexIndex++;

                p[faceVertexIndex] = v2;           
                faceVertexIndex++;

                p[faceVertexIndex] = v3;           
                faceVertexIndex++;
            }
        }
    }
}

}
