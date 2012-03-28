////////////////////////////////////////////////////////////////////////////////
// Name:      thread.cpp
// Purpose:   Implementation of class Thread
// Author:    Anton van Wezenbeek
// Copyright: (c) 2012 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#include "thread.h"
#include "fractal.h"

Thread::Thread(QObject *parent)
  : QThread(parent)
  , m_first_pass(1)
  , m_max_passes(0)
  , m_pause(false)
  , m_refresh(false)
  , m_restart(false)
  , m_stop(false)
{
}

Thread::~Thread()
{
  stop();
}

void Thread::cont()
{
  QMutexLocker locker(&m_mutex);
  
  m_pause = false;
}

void Thread::pause()
{
  m_mutex.lock();
  m_pause = true;
  m_condition.wakeOne();
  m_mutex.unlock();
}

bool Thread::render(
  const Fractal& fractal,
  const std::complex<double> & c, 
  QImage& image,
  const std::vector<uint> & colours,
  const QPoint& p,
  uint max,
  bool& converge)
{
  uint n = 0;

  if (!fractal.calc(c, n, max))
  {
    if (m_refresh && !image.isNull())
    {
      emit renderedImage(image, 0, 0, 0, true);
      QMutexLocker locker(&m_mutex);
      m_image = image;
      m_refresh = false;
    }
    
    if (m_stop)
    {
      return false;
    }
  }

  if (n < max) 
  {
    converge = false;
  } 
  
  image.setPixel(p,
   (n < max ? colours[n % colours.size()]: colours.back()));
   
  return true;
}

void Thread::render(
  const Fractal& fractal,
  const QImage& image,
  const QPointF& center, 
  double scale,
  uint first_pass,
  uint passes,
  const std::vector<uint> & colours)
{
  QMutexLocker locker(&m_mutex);

  m_center = center;
  m_scale = scale;
  m_image = image;
  m_colours = colours;
  m_first_pass = first_pass;
  m_max_passes = passes;
  m_fractal = fractal;
  m_refresh = false;
  
  if (isRunning())
  {
    m_restart = true;
    m_condition.wakeOne();
  }
}

void Thread::run()
{
  forever 
  {
    m_mutex.lock();
    
    if (m_stop)
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
      pass <= max_passes && !m_restart && !m_pause; 
      pass++)
    {
      const uint max_iterations = 16 + (8 << pass);
      
      emit renderingImage(pass, max_passes, max_iterations);
      
      bool converge = true;

      for (
        int y = -half.height(); 
        y < half.height() && !m_restart && !m_pause; 
        ++y) 
      {
        const double ay = center.y() + (y * scale);

        for (
          int x = -half.width(); 
          x < half.width() && !m_restart && !m_pause; 
          ++x) 
        {
          const double ax = center.x() + (x * scale);
          
          if (!render(
            fractal, 
            std::complex<double>(ax, ay), 
            image, 
            colours, 
            QPoint(x + half.width(), y + half.height()),
            max_iterations, 
            converge))
          {
            return;
          }
        }
      }

      if (!converge && !m_restart && !image.isNull())
      {
        emit renderedImage(image, pass, max_passes, scale, false);
      }
    }

    m_mutex.lock();
    
    if (!m_restart)
      m_condition.wait(&m_mutex);

    m_restart = false;
    m_mutex.unlock();
  }
}

void Thread::refresh()
{
  m_mutex.lock();
  m_refresh = true;
  m_condition.wakeOne();
  m_mutex.unlock();
}

void Thread::stop()
{
  m_mutex.lock();
  m_stop = true;
  m_condition.wakeOne();
  m_mutex.unlock();

  wait();
}
