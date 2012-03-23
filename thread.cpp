#include <complex>
#include "thread.h"
#include "fractal.h"

Thread::Thread(QObject *parent)
  : QThread(parent)
  , m_restart(false)
  , m_pause(false)
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
    Fractal fractal(m_fractal);
    m_mutex.unlock();
    
    const QSize half = image.size() / 2;
    
    for (uint pass = first_pass; pass <= max_passes; pass++)
    {
      const uint max_iterations = 8 << pass;
      
      emit renderingImage(pass, max_passes, max_iterations);
      
      bool converge = true;

      for (int y = -half.height(); y < half.height() && !interrupted(); ++y) 
      {
        const double ay = center.y() + (y * scale);

        for (int x = -half.width(); x < half.width() && !interrupted(); ++x) 
        {
          const double ax = center.x() + (x * scale);
          
          uint n = 0;

          if (!fractal.calc(std::complex<double>(ax, ay), n, max_iterations))
          {
            if (!m_restart)
            {
              emit renderedImage(image, scale);
            }
            
            if (m_stop)
            {
              return;
            }
          }

          if (n < max_iterations) 
          {
            converge = false;
          } 
          
          image.setPixel(
            x + half.width(), 
            y + half.height(),
           (n < max_iterations ? colours[n % colours.size()]: colours.back()));
        }
      }

      if (!converge && !m_restart)
      {
        emit renderedImage(image, scale);
      }
    }

    m_mutex.lock();
    
    if (!m_restart)
      m_condition.wait(&m_mutex);

    m_restart = false;
    m_mutex.unlock();
  }
}

void Thread::stop()
{
  m_mutex.lock();
  m_stop = true;
  m_condition.wakeOne();
  m_mutex.unlock();

  wait();
}
