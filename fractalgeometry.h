////////////////////////////////////////////////////////////////////////////////
// Name:      fractalgeometry.h
// Purpose:   Declaration of class FractalGeometry
// Author:    Anton van Wezenbeek
// Copyright: (c) 2012 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#ifndef _FRACTALGEOMETRY_H
#define _FRACTALGEOMETRY_H

#include <vector>
#include <QPointF>

// This class contains general geometry values for a fractal.
class FractalGeometry
{
public:
  // Constructor.
  FractalGeometry(
    // using this center
    const QPointF& center,
    // using this scale
    double scale,
    // pass to start with
    int firstPass,
    // using max number of passes
    int maxPasses,
    // using these colours,
    // the last colour is used for converge
    const std::vector<uint> & colours);

  QPointF m_center;
  double m_scale;
  
  int m_firstPass;
  int m_maxPasses;
  
  std::vector<uint> m_colours;
};
#endif
