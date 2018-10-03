/*
 * FrontVertex.cpp
 * aloe
 * 
 *
 */
 
#include "FrontVertex.h"

namespace alo {
     
FrontVertex::FrontVertex() :
    m_pos(0), m_id(0)
{}

FrontVertex::FrontVertex(Vector3F* pos, const Vector3F& dir, int id) :
    m_pos(pos), m_dir(dir), m_id(id)
{}

Vector3F* &FrontVertex::pos()
{ return m_pos; }

int& FrontVertex::id()
{ return m_id; }

const Vector3F* FrontVertex::pos() const
{ return m_pos; }

const Vector3F& FrontVertex::dir() const
{ return m_dir; }

const int& FrontVertex::id() const
{ return m_id; }

Vector3F FrontVertex::getPos1() const
{ return *m_pos + m_dir; }

bool FrontVertex::operator==(const FrontVertex& b) const
{ return b.id() == id(); }

}