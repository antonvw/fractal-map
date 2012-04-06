////////////////////////////////////////////////////////////////////////////////
// Name:      fractalgeometry.cpp
// Purpose:   Implementation of class FractalGeometry
// Author:    Anton van Wezenbeek
// Copyright: (c) 2012 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#include "fractalgeometry.h"

FractalGeometry::FractalGeometry(
  const QPointF& center,
  double scale,
  int firstPass,
  int maxPasses,
  const std::vector<uint> & colours)
  : m_center(center)
  , m_scale(scale)
  , m_firstPass(firstPass)
  , m_maxPasses(maxPasses)
  , m_colours(colours)
{
}

bool FractalGeometry::isOk() const
{
  return
     m_firstPass <= m_maxPasses && 
    !m_colours.empty() && 
     m_scale > 0;
}
