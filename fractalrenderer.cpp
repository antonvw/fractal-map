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
  , FractalGeometry(QPointF(0, 0), 1, 1, 0, std::vector<uint>())
  , m_state(RENDERING_INIT)
  , m_oldState(RENDERING_INIT)
{
}

FractalRenderer::~FractalRenderer()
{
  stop();
}

int FractalRenderer::calcStep(
  int pass, int first_pass, int max_passes) const
{
  int step;
  
  if (pass == max_passes)
  { 
    step = 1;
  }
  else if (pass == max_passes - 1)
  {
    step = 2;
  }
  else if (pass == first_pass)
  {
    step = 9;
  }
  else
  {
    step = 7;
  }

  return step;
}

bool FractalRenderer::end() const
{
  return 
    m_state == RENDERING_START || 
    m_state == RENDERING_PAUSED || 
    m_state == RENDERING_SKIP;
}

void FractalRenderer::pause(bool checked)
{
  QMutexLocker locker(&m_mutex);
  
  if (m_state != RENDERING_PAUSED)
  {
    m_oldState = m_state;
  }
  
  m_state = (checked ? RENDERING_PAUSED: m_oldState);
  m_condition.wakeOne();
}

void FractalRenderer::refresh()
{
  if (m_state == RENDERING_ACTIVE)
  {
    QMutexLocker locker(&m_mutex);
    m_state = RENDERING_SNAPSHOT;
    m_condition.wakeOne();
  }
}

bool FractalRenderer::render(
  const Fractal& fractal,
  const std::complex<double> & c, 
  int max,
  QImage& image,
  const QPoint& p,
  int inc,
  const std::vector<uint> & colours)
{
  int n = 0;

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
      m_state = RENDERING_ACTIVE;
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
  const FractalGeometry& geometry)
{
  if (
    !fractal.isOk() ||
    !isRunning() || 
    geometry.m_firstPass > geometry.m_maxPasses || 
    geometry.m_colours.empty() || 
    geometry.m_scale == 0 || 
    m_state == RENDERING_INIT || 
    m_state == RENDERING_PAUSED)
  {
    return false;
  }

  QMutexLocker locker(&m_mutex);
  
  m_state = RENDERING_START;
  m_image = image;
  m_fractal = fractal;
  m_center = geometry.m_center;
  m_scale = geometry.m_scale;
  m_firstPass = geometry.m_firstPass;
  m_maxPasses = geometry.m_maxPasses;
  m_colours = geometry.m_colours;
  m_fractal.setRenderer(this);
  m_condition.wakeOne();
  
  return true;
}

void FractalRenderer::run()
{
  m_state = RENDERING_READY;
  
  forever 
  {
    m_mutex.lock();
    QImage image = m_image;
    const double scale = m_scale;
    const QPointF center = m_center;
    const int first_pass = m_firstPass;
    const int max_passes = m_maxPasses;
    std::vector<uint> colours(m_colours);
    const Fractal fractal(m_fractal);
    m_mutex.unlock();
    
    const QSize half = image.size() / 2;
    
    for (
      int pass = first_pass; 
      pass <= max_passes && m_state == RENDERING_ACTIVE; 
      pass++)
    {
      const int inc = calcStep(pass, first_pass, max_passes);
      const int max_iterations = 16 + (8 << pass);
      
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

      if (!end() || m_state == RENDERING_START)
      {
        if (pass == max_passes)
        {
          switch (m_state)
          {
            case RENDERING_START: break;
            default : m_state = RENDERING_READY;
          }
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
      case RENDERING_STOPPED:
        m_mutex.unlock();
        return;
      default: 
        m_state = RENDERING_ACTIVE;
        break;
    }
        
    m_mutex.unlock();
  }
}

void FractalRenderer::skip()
{
  if (m_state == RENDERING_ACTIVE)
  {
    QMutexLocker locker(&m_mutex);
    m_state = RENDERING_SKIP;
    m_condition.wakeOne();
  }
}

void FractalRenderer::start()
{
  QThread::start();
}

void FractalRenderer::stop()
{
  m_mutex.lock();
  m_state = RENDERING_STOPPED;
  m_condition.wakeOne();
  m_mutex.unlock();

  wait();
}
