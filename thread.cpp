#include <math.h>
#include "thread.h"

enum
{
  FRACTAL_MANDELBROTSET = 0,
  FRACTAL_JULIASET_1,
  FRACTAL_JULIASET_2,
  FRACTAL_JULIASET_3,
  FRACTAL_JULIASET_4,
  FRACTAL_JULIASET_5,
  FRACTAL_JULIASET_6,
  FRACTAL_JULIASET_7,
  FRACTAL_JULIASET_8,
  FRACTAL_JULIASET_MIS,
};

std::vector<QString> Thread::m_fractals;

Thread::Thread(QObject *parent)
  : QThread(parent)
  , m_restart(false)
  , m_stop(false)
  , m_fractal(FRACTAL_MANDELBROTSET)
{
  if (m_fractals.empty())
  {
    m_fractals.push_back("mandelbrot set");
    m_fractals.push_back("julia set 1 golden ratio");
    m_fractals.push_back("julia set 2");
    m_fractals.push_back("julia set 3");
    m_fractals.push_back("julia set 4");
    m_fractals.push_back("julia set 5");
    m_fractals.push_back("julia set 6");
    m_fractals.push_back("julia set 7");
    m_fractals.push_back("julia set 8 dragon");
    m_fractals.push_back("julia set mis");
  }
}

Thread::~Thread()
{
  stop();
}

bool Thread::fractal(double ax, double ay, uint& n, uint max, uint diverge, int type)
{ 
  switch (type)
  {
    case FRACTAL_JULIASET_1:
    {
      const double phi = 1.6180339887498948482;
      return julia(std::complex<double>(1 - phi, 0), ax, ay, n, max, diverge);
    }
    break;
    
    case FRACTAL_JULIASET_2:
      return julia(std::complex<double>(-0.4, 0.6), ax, ay, n, max, diverge);
    break;
    
    case FRACTAL_JULIASET_3:
      return julia(std::complex<double>(0.285, 0), ax, ay, n, max, diverge);
    break;
    
    case FRACTAL_JULIASET_4:
      return julia(std::complex<double>(0.285, 0.01), ax, ay, n, max, diverge);
    break;
    
    case FRACTAL_JULIASET_5:
      return julia(std::complex<double>(0.45, 0.1428), ax, ay, n, max, diverge);
    break;
    
    case FRACTAL_JULIASET_6:
      return julia(std::complex<double>(-0.70176, -0.3842), ax, ay, n, max, diverge);
    break;
    
    case FRACTAL_JULIASET_7:
      return julia(std::complex<double>(-0.835, -0.2321), ax, ay, n, max, diverge);
    break;
    
    case FRACTAL_JULIASET_8:
      return julia(std::complex<double>(-0.8, 0.156), ax, ay, n, max, diverge);
    break;
    
    case FRACTAL_JULIASET_MIS:
      return julia(std::complex<double>(0, 1), ax, ay, n, max, diverge);
    break;
    
    case FRACTAL_MANDELBROTSET:
    {
      const std::complex<double> c(ax, ay);
      std::complex<double> z;
    
      for (n = 0; n < max && !m_stop; n++)
      {
        z = z * z - c;
        
        if (abs(z) > diverge)
        {
          break;
        }
      }
    }
    break;
  }
  
  return !m_stop;
}          

bool Thread::julia(const std::complex<double> & c, double ax, double ay, uint& n, uint max, uint diverge)
{
  std::complex<double> z(ax, ay);
    
  for (n = 0; n < max && !m_stop; n++)
  {
    z = z * z + c;
        
    if (abs(z) > diverge)
    {
      break;
    }
  }
  
  return !m_stop;
}

void Thread::render(
  const QString& fractal,
  const QPointF& center, 
  double scale,
  const QImage& image,
  uint first_pass,
  uint passes,
  const std::vector<uint> & colours,
  const double diverge)
{
  QMutexLocker locker(&m_mutex);

  bool found = false;
  
  for (uint i = 0; i < m_fractals.size() && !found; i++)
  {
    if (m_fractals[i] == fractal)
    {
      m_fractal = i;
      found = true;
    }
  }
  
  if (!found)
  {
    return;
  }
  
  m_center = center;
  m_scale = scale;
  m_image = image;
  m_colours = colours;
  m_first_pass = first_pass;
  m_max_passes = passes;
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
    const double diverge = m_diverge;
    const int type = m_fractal;
    m_mutex.unlock();
    
    const QSize half = image.size() / 2;
    
    for (uint pass = first_pass; pass <= max_passes; pass++)
    {
      const uint max_iterations = 8 << pass;
      
      emit renderingImage(pass, max_passes, max_iterations);
      
      bool converge = true;

      for (int y = -half.height(); y < half.height() && !m_restart; ++y) 
      {
        const double ay = center.y() + (y * scale);

        for (int x = -half.width(); x < half.width() && !m_restart; ++x) 
        {
          const double ax = center.x() + (x * scale);
          
          uint n = 0;

          if (!fractal(ax, ay, n, max_iterations, diverge, type))
          {
            emit renderedImage(image, scale);
            return;
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
