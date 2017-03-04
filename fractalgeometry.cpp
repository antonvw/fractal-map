////////////////////////////////////////////////////////////////////////////////
// Name:      fractalgeometry.cpp
// Purpose:   Implementation of class FractalControl
// Author:    Anton van Wezenbeek
// Copyright: (c) 2017 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#include "fractalgeometry.h"
#include "fractalwidget.h"

FractalGeometry::FractalGeometry(
  const QwtInterval& xInterval,
  const QwtInterval& yInterval,
  int depth,
  const QString& dir)
  : m_intervalX(xInterval)
  , m_intervalY(yInterval)
  , m_dir(dir)
  , m_coloursMinWave(min_wave)
  , m_coloursMaxWave(max_wave)
  , m_depth(depth)
{
}

bool FractalGeometry::isOk() const
{
  return
    m_intervalX.isValid() &&
    m_intervalY.isValid() &&
  ((!m_useImages && !m_colours.empty()) || (m_useImages && !m_images.empty()));
}

bool FractalGeometry::setColour(const QColor& color)
{
  if (!color.isValid())
  {
    return false;
  }

  m_finished = false;
    
  if (m_colours[m_colourIndex] != color.rgb())
  {
    m_colours[m_colourIndex] = color.rgb();  
  }
  
  if (m_colourIndexFromStart)
  {
    if ((unsigned int)m_colourIndex < m_colours.size() - 1)
    {
      m_colourIndex++;
    }
    else
    {
      m_finished = true;
    }
  }
  else
  {
    if (m_colourIndex >= 1)
    {
      m_colourIndex--;
    }
    else
    {
      m_finished = true;
    }
  }

  return true;
}

void FractalGeometry::setColours(int colours)
{
  m_colours.clear();
  
  const double visible_min = m_coloursMinWave;
  const double visible_max = m_coloursMaxWave;

  for (int i = 0; i < colours - 1; ++i)
  {
    m_colours.push_back(
      wav2RGB(visible_min + (i * (visible_max - visible_min) / colours)));
  }
  
  m_colours.push_back(qRgb(0, 0, 0));
}

// see
// http://codingmess.blogspot.com/2009/05/conversion-of-wavelength-in-nanometers.html
uint FractalGeometry::wav2RGB(double w) const
{
  double R = 0.0;
  double G = 0.0;
  double B = 0.0;
  
  if (w >= min_wave && w < 440)
  {
    R = -(w - 440) / (440 - 350);
    G = 0.0;
    B = 1.0;
  }
  else if (w >= 440 && w < 490)
  {
    R = 0.0;
    G = (w - 440) / (490 - 440);
    B = 1.0;
  }
  else if (w >= 490 && w < 510)
  {
    R = 0.0;
    G = 1.0;
    B = -(w - 510) / (510 - 490);
  }
  else if (w >= 510 && w < 580)
  {
    R = (w - 510) / (580 - 510);
    G = 1.0;
    B = 0.0;
  }
  else if (w >= 580 && w < 645)
  {
    R = 1.0;
    G = -(w - 645) / (645 - 580);
    B = 0.0;
  }
  else if (w >= 645 && w <= max_wave)
  {
    R = 1.0;
    G = 0.0;
    B = 0.0;
  }

  // intensity correction
  double SSS = 0;
  
  if (w >= min_wave && w < 420)
    SSS = 0.3 + 0.7*(w - 350) / (420 - 350);
  else if (w >= 420 && w <= 700)
    SSS = 1.0;
  else if (w > 700 && w <= max_wave)
    SSS = 0.3 + 0.7*(max_wave - w) / (max_wave - 700);
      
  SSS *= 255;

  return qRgb(int(SSS*R), int(SSS*G), int(SSS*B));
}
