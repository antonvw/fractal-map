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
  , m_state(RENDERING_INIT)
  , m_oldState(RENDERING_INIT)
{
}

FractalRenderer::~FractalRenderer()
{
  stop();
}

bool FractalRenderer::allowRender() const
{
  return
    m_state == RENDERING_READY || 
    m_state == RENDERING_ACTIVE ||
    m_state == RENDERING_RESET;
}

const QSize FractalRenderer::calcStep(int pass, const FractalGeometry& geo) const
{
  if (geo.useImages())
  {
    return QSize(geo.images().front().width(), geo.images().front().height());
  }
  
  int step;
  
  if (geo.singlePass())
  {
    step = 1;
  }
  else if (pass == geo.maxPasses())
  { 
    step = 1;
  }
  else if (pass == geo.maxPasses() - 1)
  {
    step = 2;
  }
  else if (pass == geo.firstPass())
  {
    step = 9;
  }
  else
  {
    step = 7;
  }

  return QSize(step, step);
}

void FractalRenderer::cont()
{
  if (m_state == RENDERING_PAUSED || m_state == RENDERING_INTERRUPT)
  {
    QMutexLocker locker(&m_mutex);
    m_state = m_oldState;
    m_condition.wakeOne();
  }
}

bool FractalRenderer::end() const
{
  return 
    m_state == RENDERING_START || 
    m_state == RENDERING_INTERRUPT || 
    m_state == RENDERING_PAUSED || 
    m_state == RENDERING_RESET || 
    m_state == RENDERING_SKIP;
}

void FractalRenderer::interrupt()
{
  if (m_state == RENDERING_ACTIVE)
  {
    QMutexLocker locker(&m_mutex);
    m_oldState = m_state;
    m_state = RENDERING_INTERRUPT;
    m_condition.wakeOne();
  }
}

bool FractalRenderer::interrupted() const 
{
  return 
    m_state == RENDERING_PAUSED || 
    m_state == RENDERING_INTERRUPT || 
    m_state == RENDERING_RESET || 
    m_state == RENDERING_START || 
    m_state == RENDERING_SKIP || 
    m_state == RENDERING_SNAPSHOT || 
    m_state == RENDERING_STOPPED;
}
      
bool FractalRenderer::nextStateForCalcEnd(const QImage& image)
{
  switch (m_state)
  {
  case RENDERING_INTERRUPT:
  case RENDERING_PAUSED:
    {
    QMutexLocker locker(&m_mutex);
    m_condition.wait(&m_mutex);
    }
    break;
      
  case RENDERING_SNAPSHOT:
    {
    emit rendered(image, m_state);
    QMutexLocker locker(&m_mutex);
    m_image = image;
    m_state = RENDERING_ACTIVE;
    }
    break;
    
  case RENDERING_STOPPED: return false; 
    break;
  }
  
  return true;
}
    
void FractalRenderer::pause()
{
  if (m_state != RENDERING_PAUSED)
  {
    QMutexLocker locker(&m_mutex);
    m_oldState = m_state;
    m_state = RENDERING_PAUSED;
    m_condition.wakeOne();
  }
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
  const std::vector<QImage> & images)
{
  int n = 0;

  const bool result = fractal.calc(c, n, max);
  
  // We have to use image n.
  const int im = (n < max ? (n % images.size()): images.size() - 1);
  
  for (int i = 0; i < images[im].width(); i++)
  {
    for (int j = 0; j < images[im].height(); j++)
    {
      const QPoint pos(p + QPoint(i, j));
      
      if (image.valid(pos))
      {
        image.setPixel(pos, images[im].pixel(QPoint(i, j)));
      }
    }
  }
  
  if (!result)
  {
    return nextStateForCalcEnd(image);
  }
   
  return true;
}

bool FractalRenderer::render(
  const Fractal& fractal,
  const std::complex<double> & c, 
  int max,
  QImage& image,
  const QPoint& p,
  const QSize& inc,
  const std::vector<uint> & colours)
{
  int n = 0;

  const bool result = fractal.calc(c, n, max);
  
  for (int i = 0; i < inc.width(); i++)
  {
    for (int j = 0; j < inc.height(); j++)
    {
      const QPoint pos(p + QPoint(i, j));
      
      if (image.valid(pos))
      {
        image.setPixel(pos,
         (n < max ? colours[n % colours.size()]: colours.back()));
      }
    }
  }
  
  if (!result)
  {
    return nextStateForCalcEnd(image);
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
    !geometry.isOk() ||
    !allowRender())
  {
    return false;
  }

  QMutexLocker locker(&m_mutex);
  
  m_state = RENDERING_START;
  m_image = image;
  m_fractal = fractal;
  m_geo = geometry;
  m_fractal.setRenderer(this);
  m_condition.wakeOne();
    
  return true;
}

void FractalRenderer::reset()
{
  if (m_state == RENDERING_INTERRUPT)
  {
    QMutexLocker locker(&m_mutex);
    m_state = RENDERING_RESET;
    m_condition.wakeOne();
  }
}

void FractalRenderer::run()
{
  m_state = RENDERING_READY;
  
  forever 
  {
    m_mutex.lock();
    QImage image = m_image;
    const FractalGeometry geo(m_geo);
    const Fractal fractal(m_fractal);
    m_mutex.unlock();
    
    for (
      int pass = (!geo.singlePass() ? geo.firstPass(): geo.maxPasses()); 
      pass <= geo.maxPasses() && m_state == RENDERING_ACTIVE; 
      pass++)
    {
      const QSize inc = calcStep(pass, geo);
      const int max_iterations = 16 + (8 << pass);
      
      emit rendering(
        geo.singlePass()? -1: pass, 
        geo.singlePass() ? -1: geo.maxPasses(), 
        max_iterations);
      
      for (
        int y = 0; 
        y < image.height() && !end();
        y+= inc.height())
      {
        emit rendering(y, image.height());
        
        const double cy = geo.intervalY().maxValue() - 
          (((double)y / image.height()) * geo.intervalY().width());

        for (
          int x = 0; 
          x < image.width() && !end();
          x+= inc.width()) 
        {
          const double cx = 
            geo.intervalX().minValue() + 
            (((double)x / image.width()) * geo.intervalX().width());
          
          if (!geo.useImages())
          {
            if (!render(
              fractal, 
              std::complex<double>(cx, cy), 
              max_iterations, 
              image, 
              QPoint(x, y),
              inc,
              geo.colours()))
            {
              return;
            }
          }
          else
          {
            if (!render(
              fractal, 
              std::complex<double>(cx, cy), 
              max_iterations, 
              image, 
              QPoint(x, y),
              geo.images()))
            {
              return;
            }
          }
        }
      }

      if (!end() || m_state == RENDERING_START)
      {
        if (pass == geo.maxPasses())
        {
          switch (m_state)
          {
            case RENDERING_START: break;
            default : m_state = RENDERING_READY;
          }
        }
         
        emit rendered(image, m_state);
      }
    }

    m_mutex.lock();
    
    switch (m_state)
    {
      case RENDERING_INTERRUPT:
      case RENDERING_PAUSED:
      case RENDERING_READY:
      case RENDERING_RESET:
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
