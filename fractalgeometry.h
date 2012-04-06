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
  // Default constructor.
  FractalGeometry(
    // using this center
    const QPointF& center = QPointF(0,0),
    // using this scale
    double scale = 0,
    // pass to start with
    int firstPass = 0,
    // using max number of passes
    int maxPasses = 0,
    // using these colours,
    // the last colour is used for converge
    const std::vector<uint> & colours = std::vector<uint>());
    
  // Returns true if parameters are ok.
  bool isOk() const;

  QPointF m_center;
  double m_scale;
  
  int m_firstPass;
  int m_maxPasses;
  
  std::vector<uint> m_colours;
};
#endif
