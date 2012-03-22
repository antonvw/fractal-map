#include <math.h>
#include "fractal.h"
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

std::vector<QString> Fractal::m_names;

Fractal::Fractal(
  Thread* thread, 
  const QString& name,
  uint diverge)
  : m_isOk(false)
  , m_diverge(diverge)
  , m_type(FRACTAL_MANDELBROTSET)
  , m_thread(thread)
{
  for (uint i = 0; i < m_names.size() && !m_isOk; i++)
  {
    if (m_names[i] == name)
    {
      m_type = i;
      m_isOk = true;
    }
  }
}

bool Fractal::calc(
  double ax, double ay, uint& n, uint max)
{ 
  switch (m_type)
  {
    case FRACTAL_JULIASET_1:
    {
      const double phi = 1.6180339887498948482;
      return julia(std::complex<double>(1 - phi, 0), ax, ay, n, max);
    }
    break;
    
    case FRACTAL_JULIASET_2:
      return julia(std::complex<double>(-0.4, 0.6), ax, ay, n, max);
    break;
    
    case FRACTAL_JULIASET_3:
      return julia(std::complex<double>(0.285, 0), ax, ay, n, max);
    break;
    
    case FRACTAL_JULIASET_4:
      return julia(std::complex<double>(0.285, 0.01), ax, ay, n, max);
    break;
    
    case FRACTAL_JULIASET_5:
      return julia(std::complex<double>(0.45, 0.1428), ax, ay, n, max);
    break;
    
    case FRACTAL_JULIASET_6:
      return julia(std::complex<double>(-0.70176, -0.3842), ax, ay, n, max);
    break;
    
    case FRACTAL_JULIASET_7:
      return julia(std::complex<double>(-0.835, -0.2321), ax, ay, n, max);
    break;
    
    case FRACTAL_JULIASET_8:
      return julia(std::complex<double>(-0.8, 0.156), ax, ay, n, max);
    break;
    
    case FRACTAL_JULIASET_MIS:
      return julia(std::complex<double>(0, 1), ax, ay, n, max);
    break;
    
    case FRACTAL_MANDELBROTSET:
    {
      const std::complex<double> c(ax, ay);
      std::complex<double> z;
    
      for (n = 0; n < max && !m_thread->interrupted(); n++)
      {
        z = z * z - c;
        
        if (abs(z) > m_diverge)
        {
          break;
        }
      }
    }
    break;
  }
  
  return !m_thread->interrupted();
}          

bool Fractal::julia(
  const std::complex<double> & c, 
  double ax, double ay, uint& n, uint max)
{
  std::complex<double> z(ax, ay);
    
  for (n = 0; n < max && !m_thread->interrupted(); n++)
  {
    z = z * z + c;
        
    if (abs(z) > m_diverge)
    {
      break;
    }
  }
  
  return !m_thread->interrupted();
}

std::vector<QString> & Fractal::names()
{
  if (m_names.empty())
  {
    m_names.push_back("mandelbrot set");
    m_names.push_back("julia set 1 golden ratio");
    m_names.push_back("julia set 2");
    m_names.push_back("julia set 3");
    m_names.push_back("julia set 4");
    m_names.push_back("julia set 5");
    m_names.push_back("julia set 6");
    m_names.push_back("julia set 7");
    m_names.push_back("julia set 8 dragon");
    m_names.push_back("julia set mis");
  }

  return m_names;  
}
