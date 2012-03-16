//#include <complex>
#include <math.h>
#include "renderthread.h"

RenderThread::RenderThread(QObject *parent)
  : QThread(parent)
{
  m_restart = false;
  m_abort = false;

}

RenderThread::~RenderThread()
{
  m_mutex.lock();
  m_abort = true;
  m_condition.wakeOne();
  m_mutex.unlock();

  wait();
}

void RenderThread::render(
  const QPointF& center, 
  double scale,
  const QSize& size,
  int colormap_size)
{
  QMutexLocker locker(&m_mutex);

  m_center = center;
  m_scale = scale;
  m_size = size;
  m_ColormapSize = colormap_size;
  
  for (int i = 0; i < m_ColormapSize; ++i)
    m_colormap.push_back(rgbFromWaveLength(380.0 + (i * 400.0 / m_ColormapSize)));

  if (!isRunning())
  {
    start(LowPriority);
  } 
  else 
  {
    m_restart = true;
    m_condition.wakeOne();
  }
}

void RenderThread::run()
{
  forever 
  {
    m_mutex.lock();
    const QSize size = m_size;
    const double scale = m_scale;
    const QPointF center = m_center;
    const uint colormapSize = m_ColormapSize;
    m_mutex.unlock();
    
    const int halfWidth = size.width() / 2;
    const int halfHeight = size.height() / 2;
    
    QImage image(size, QImage::Format_RGB32);

    const int NumPasses = 8;
    
    for (int pass = 0; pass < NumPasses; pass++)
    {
      const int MaxIterations = (1 << (2 * pass + 6)) + 32;
      const int Limit = 2;
      
      emit renderingImage(pass + 1, NumPasses, MaxIterations);
      
      bool allBlack = true;

      for (int y = -halfHeight; y < halfHeight && !m_restart; ++y) 
      {
        if (m_abort)
          return;

        uint *scanLine =
          reinterpret_cast<uint *>(image.scanLine(y + halfHeight));
          
        const double ay = center.y() + (y * scale);

        for (int x = -halfWidth; x < halfWidth; ++x) 
        {
          const double ax = center.x() + (x * scale);
          
          /*
          const std::complex<double> c(ax, ay);
          std::complex<double> z;
          
          int n = 0;
          
          for (n = 0; n < MaxIterations && abs(z) < Limit; n++)
          {
            z = z * z - c;
          }
          */
          
          double zx = 0;
          double zy = 0;
          
          int n = 0;
          
          for (n = 0; n < MaxIterations; n++)
          {
            const double zxn = zx * zx - zy * zy - ax;
            const double zyn = 2 * zx * zy - ay;
            
            zx = zxn;
            zy = zyn;
            
            if (zx * zx - zy * zy > Limit)
            {
              break;
            }
          }

          if (n < MaxIterations) 
          {
            *scanLine++ = m_colormap[n % colormapSize];
            allBlack = false;
          } 
          else 
          {
            *scanLine++ = qRgb(0, 0, 0);
          }
        }
      }

      if (!allBlack && !m_restart)
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

uint RenderThread::rgbFromWaveLength(double wave)
{
  double r = 0.0;
  double g = 0.0;
  double b = 0.0;

  if (wave >= 380.0 && wave <= 440.0) 
  {
    r = -1.0 * (wave - 440.0) / (440.0 - 380.0);
    b = 1.0;
  } 
  else if (wave >= 440.0 && wave <= 490.0) 
  {
    g = (wave - 440.0) / (490.0 - 440.0);
    b = 1.0;
  } 
  else if (wave >= 490.0 && wave <= 510.0) 
  {
    g = 1.0;
    b = -1.0 * (wave - 510.0) / (510.0 - 490.0);
  } 
  else if (wave >= 510.0 && wave <= 580.0) 
  {
    r = (wave - 510.0) / (580.0 - 510.0);
    g = 1.0;
  } 
  else if (wave >= 580.0 && wave <= 645.0) 
  {
    r = 1.0;
    g = -1.0 * (wave - 645.0) / (645.0 - 580.0);
  } 
  else if (wave >= 645.0 && wave <= 780.0) 
  {
    r = 1.0;
  }

  double s = 1.0;
  
  if (wave > 700.0)
    s = 0.3 + 0.7 * (780.0 - wave) / (780.0 - 700.0);
  else if (wave <  420.0)
    s = 0.3 + 0.7 * (wave - 380.0) / (420.0 - 380.0);

  r = pow(r * s, 0.8);
  g = pow(g * s, 0.8);
  b = pow(b * s, 0.8);
  
  return qRgb(int(r * 255), int(g * 255), int(b * 255));
}
