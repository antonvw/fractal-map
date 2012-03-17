#include <complex>
#include <math.h>
#include "thread.h"

Thread::Thread(QObject *parent)
  : QThread(parent)
  , m_restart(false)
  , m_stop(false)
{
}

Thread::~Thread()
{
  m_mutex.lock();
  m_stop = true;
  m_condition.wakeOne();
  m_mutex.unlock();

  wait();
}

void Thread::render(
  const QPointF& center, 
  double scale,
  const QSize& size,
  uint passes,
  const std::vector<uint> & colours,
  const double diverge)
{
  QMutexLocker locker(&m_mutex);

  m_center = center;
  m_scale = scale;
  m_size = size;
  m_colours = colours;
  m_passes = passes;
  m_diverge = diverge;
  
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
    const QSize size = m_size;
    const double scale = m_scale;
    const QPointF center = m_center;
    const uint passes = m_passes;
    std::vector<uint> colours(m_colours);
    const double diverge = m_diverge;
    m_mutex.unlock();
    
    const QSize half = size / 2;
    
    QImage image(size, QImage::Format_RGB32);

    for (uint pass = 0; pass < passes; pass++)
    {
      const uint maxIterations = 8 << pass;
      
      emit renderingImage(pass + 1, passes, maxIterations);
      
      bool converge = true;

      for (int y = -half.height(); y < half.height() && !m_restart; ++y) 
      {
        if (m_stop)
          return;

        uint *scanLine =
          reinterpret_cast<uint *>(image.scanLine(y + half.height()));
          
        const double ay = center.y() + (y * scale);

        for (int x = -half.width(); x < half.width(); ++x) 
        {
          const double ax = center.x() + (x * scale);
          
          const std::complex<double> c(ax, ay);
          std::complex<double> z;
          
          uint n = 0;
          
          for (n = 0; n < maxIterations && abs(z) < diverge; n++)
          {
            z = z * z - c;
          }

          if (n < maxIterations) 
          {
            converge = false;
          } 
          
          *scanLine++ = 
            (n < maxIterations ? colours[n % colours.size()]: colours.back());
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
