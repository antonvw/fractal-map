////////////////////////////////////////////////////////////////////////////////
// Name:      fractalrenderer.cpp
// Purpose:   Implementation of class FractalRenderer
// Author:    Anton van Wezenbeek
// Copyright: (c) 2017 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#include "fractalrenderer.h"
#include "fractal.h"

FractalRenderer::FractalRenderer(QObject *parent)
  : QThread(parent)
{
}

FractalRenderer::~FractalRenderer()
{
  stop();
}

const QSize FractalRenderer::calcStep(const FractalGeometry& geo) const
{
  if (geo.useImages())
  {
    return QSize(geo.images().front().width(), geo.images().front().height());
  }
  
  return QSize(1, 1);
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
  QMutexLocker locker(&m_mutex);
  m_state = RENDERING_SNAPSHOT;
  m_condition.wakeOne();
}

bool FractalRenderer::render(
  const Fractal& fractal,
  const std::complex<double> & c, 
  const FractalGeometry& geo,
  QImage& image,
  const QPoint& p,
  const QSize& inc)
{
  if (geo.useImages())
  {
    if (geo.images().empty()) return false;
  }
  else
  {
    if (geo.colours().empty()) return false;
  }

  int n = 0;

  const bool result = fractal.calc(c, n, geo.depth());
  const int ii = (geo.useImages() ? 
    (n < geo.depth() ? (n % geo.images().size()): geo.images().size() - 1): 0);
  const auto height(!geo.useImages() ? inc.height(): geo.image(ii).height());
  const auto width(!geo.useImages() ? inc.width(): geo.image(ii).width());
  
  for (int h = 0; h < height; h++)
  {
    for (int w = 0; w < width; w++)
    {
      const QPoint pos(p + QPoint(w, h));
      
      if (image.valid(pos))
      {
        image.setPixel(pos, 
          geo.useImages() ? 
            geo.image(ii).pixel(QPoint(w, h)):
             (n < geo.depth() ? 
               geo.colour(n % geo.colours().size()): 
               geo.colours().back()));
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
  if (!fractal.isOk() || !geometry.isOk())
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

void FractalRenderer::restart()
{
  QMutexLocker locker(&m_mutex);
  m_state = RENDERING_START;
  m_condition.wakeOne();
}

void FractalRenderer::run()
{
  m_state = RENDERING_ACTIVE;

  forever
  {
    m_mutex.lock();
    QImage image = m_image;
    const FractalGeometry geo(m_geo);
    const Fractal fractal(m_fractal);
    m_mutex.unlock();
    
    std::complex<double> c;
    const QSize inc = calcStep(geo);

    for (int y = 0; y < image.height() && !interrupted(); y+= inc.height())
    {
      emit rendering(y, image.height());
      
      c.imag(geo.intervalY().maxValue() - 
        (((double)y / image.height()) * geo.intervalY().width()));

      for (int x = 0; x < image.width() && !interrupted(); x+= inc.width()) 
      {
        c.real(geo.intervalX().minValue() + 
          (((double)x / image.width()) * geo.intervalX().width()));
        
        if (!render(fractal, c, geo, image, QPoint(x, y), inc))
        {
          return;
        }
      }
    }

    m_mutex.lock();

    if (!interrupted())
    {
      m_state = RENDERING_READY;
    }

    if (!image.isNull())
    {
      emit rendered(image, m_state);
    }

    switch (m_state)
    {
      case RENDERING_INTERRUPT:
      case RENDERING_PAUSED:
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

void FractalRenderer::stop()
{
  m_mutex.lock();
  m_state = RENDERING_STOPPED;
  m_condition.wakeOne();
  m_mutex.unlock();

  wait();
}
