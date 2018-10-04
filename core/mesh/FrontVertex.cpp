/*
 * FrontVertex.cpp
 * aloe
 * 
 *
 */
 
#include "FrontVertex.h"

namespace alo {
     
FrontVertex::FrontVertex() :
    m_pos(0), m_dir(0.f, 0.f, 0.f), m_id(0), m_curvature(0.f)
{}

FrontVertex::FrontVertex(Vector3F* pos, int id) :
    m_pos(pos), m_dir(0.f, 0.f, 0.f), m_id(id), m_curvature(0.f)
{}

Vector3F& FrontVertex::dir()
{ return m_dir; }

Vector3F* &FrontVertex::pos()
{ return m_pos; }

int& FrontVertex::id()
{ return m_id; }

float& FrontVertex::curvature()
{ return m_curvature; }

const Vector3F* FrontVertex::pos() const
{ return m_pos; }

const Vector3F& FrontVertex::dir() const
{ return m_dir; }

const int& FrontVertex::id() const
{ return m_id; }

const float& FrontVertex::curvature() const
{ return m_curvature; }

void FrontVertex::modifyDir(const Vector3F& v)
{ m_dir = m_dir * .5f + v * .5f; }

bool FrontVertex::operator==(const FrontVertex& b) const
{ return b.id() == id(); }

}