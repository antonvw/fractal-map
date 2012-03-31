////////////////////////////////////////////////////////////////////////////////
// Name:      fractalrenderer.cpp
// Purpose:   Implementation of class FractalRenderer
// Author:    Anton van Wezenbeek
// Copyright: (c) 2012 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#include "fractalrenderer.h"
#include "fractal.h"

FractalRenderer::FractalRenderer(QObject *parent)
  : QThread(parent)
  , m_scale(1)
  , m_first_pass(1)
  , m_max_passes(0)
  , m_state(RENDERING_INIT)
{
}

FractalRenderer::~FractalRenderer()
{
  stop();
}

bool FractalRenderer::end() const
{
  return m_state == RENDERING_RESTART || m_state == RENDERING_PAUSED || m_state == RENDERING_SKIP;
}

void FractalRenderer::pause(bool checked)
{
  QMutexLocker locker(&m_mutex);
  m_state = (checked ? RENDERING_PAUSED: RENDERING_IN_PROGRESS);
  m_condition.wakeOne();
}

void FractalRenderer::refresh()
{
  QMutexLocker locker(&m_mutex);
  m_state = RENDERING_SNAPSHOT;
  m_condition.wakeOne();
}

bool FractalRenderer::render(
  const Fractal& fractal,
  const std::complex<double> & c, 
  uint max,
  QImage& image,
  const QPoint& p,
  int inc,
  const std::vector<uint> & colours)
{
  uint n = 0;

  const bool result = fractal.calc(c, n, max);
  
  for (int i = 0; i < inc; i++)
  {
    for (int j = 0; j < inc; j++)
    {
      image.setPixel(p + QPoint(i, j),
       (n < max ? colours[n % colours.size()]: colours.back()));
    }
  }
  
  if (!result)
  {
    switch (m_state)
    {
    case RENDERING_PAUSED:
      {
      QMutexLocker locker(&m_mutex);
      m_condition.wait(&m_mutex);
      }
      break;
    case RENDERING_SNAPSHOT:
      {
      emit rendered(image, 0, m_state);
      QMutexLocker locker(&m_mutex);
      m_image = image;
      m_state = RENDERING_IN_PROGRESS;
      }
      break;
    
    case RENDERING_STOPPED: return false; 
      break;
    }
  }
   
  return true;
}

bool FractalRenderer::render(
  const Fractal& fractal,
  const QImage& image,
  const QPointF& center, 
  double scale,
  uint first_pass,
  uint passes,
  const std::vector<uint> & colours)
{
  QMutexLocker locker(&m_mutex);
  
  if (first_pass > passes || colours.empty() || scale == 0 || m_state == RENDERING_PAUSED)
  {
    return false;
  }

  m_center = center;
  m_scale = scale;
  m_image = image;
  m_colours = colours;
  m_first_pass = first_pass;
  m_max_passes = passes;
  m_fractal = fractal;
  
  if (isRunning())
  {
    m_state = RENDERING_RESTART;
    m_condition.wakeOne();
  }
  
  return true;
}

void FractalRenderer::run()
{
  m_state = RENDERING_READY;
  
  forever 
  {
    m_mutex.lock();
    
    if (m_state == RENDERING_STOPPED)
    {
      m_mutex.unlock();
      return;
    }
    
    QImage image = m_image;
    const double scale = m_scale;
    const QPointF center = m_center;
    const uint first_pass = m_first_pass;
    const uint max_passes = m_max_passes;
    std::vector<uint> colours(m_colours);
    const Fractal fractal(m_fractal);
    m_mutex.unlock();
    
    const QSize half = image.size() / 2;
    
    for (
      uint pass = first_pass; 
      pass <= max_passes && m_state != RENDERING_RESTART && m_state != RENDERING_PAUSED; 
      pass++)
    {
      if (m_state == RENDERING_SKIP)
      {
        m_state = RENDERING_IN_PROGRESS;
      }
      
      int inc;
      
      if (pass == max_passes)
      { 
        inc = 1;
      }
      else if (pass == max_passes - 1)
      {
        inc = 2;
      }
      else
      {
        inc = 7;
      }
      
      const uint max_iterations = 16 + (8 << pass);
      
      emit rendering(pass, max_passes, max_iterations);
      
      for (
        int y = 0; 
        y < image.height() && !end();
        y+= inc)
      {
        emit rendering(y, image.height());

        const double cy = center.y() + ((y - half.height())* scale);
        
        for (
          int x = 0; 
          x < image.width() && !end();
          x+= inc) 
        {
          const double cx = center.x() + ((x - half.width())* scale);
          
          if (!render(
            fractal, 
            std::complex<double>(cx, cy), 
            max_iterations, 
            image, 
            QPoint(x, y),
            inc,
            colours))
          {
            return;
          }
        }
      }

      if (!end())
      {
        if (pass == max_passes)
        {
          m_state = RENDERING_READY;
        }
         
        emit rendered(image, scale, m_state);
      }
    }

    m_mutex.lock();
    
    switch (m_state)
    {
      case RENDERING_PAUSED:
        break;
      case RENDERING_READY:
        m_condition.wait(&m_mutex);
        break;
      default: 
        m_state = RENDERING_IN_PROGRESS;
        break;
    }
        
    m_mutex.unlock();
  }
}

void FractalRenderer::skip()
{
  QMutexLocker locker(&m_mutex);
  m_state = RENDERING_SKIP;
  m_condition.wakeOne();
}

void FractalRenderer::stop()
{
  m_mutex.lock();
  m_state = RENDERING_STOPPED;
  m_condition.wakeOne();
  m_mutex.unlock();

  wait();
}
