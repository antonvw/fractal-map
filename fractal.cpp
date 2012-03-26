#include <math.h>
#include "fractal.h"
#include "thread.h"

enum
{
  FRACTAL_MANDELBROTSET = 0,
  FRACTAL_JULIASET,
  FRACTAL_JULIASET_1,
  FRACTAL_JULIASET_2,
  FRACTAL_JULIASET_3,
  FRACTAL_JULIASET_4,
  FRACTAL_JULIASET_5,
  FRACTAL_JULIASET_6,
  FRACTAL_JULIASET_7,
  FRACTAL_JULIASET_8,
  FRACTAL_JULIASET_9,
  FRACTAL_GLYNN,
};

std::vector<QString> Fractal::m_names;

Fractal::Fractal(
  Thread* thread, 
  const QString& name,
  uint diverge,
  const std::complex<double> & c,
  double exp)
  : m_isOk(false)
  , m_diverge(diverge)
  , m_name(name)
  , m_thread(thread)
  , m_julia(c)
  , m_julia_exponent(exp)
{
  int type = FRACTAL_MANDELBROTSET;
  
  for (uint i = 0; i < m_names.size() && !m_isOk; i++)
  {
    if (m_names[i] == name)
    {
      type = i;
      m_isOk = true;
    }
  }
  
  if (!m_isOk)
  {
    return;
  }
  
  switch (type)
  {
    case FRACTAL_JULIASET_1:
    {
      const double phi = 1.6180339887498948482;
      m_julia = std::complex<double>(1 - phi, 0);
    }
    break;
    
    case FRACTAL_JULIASET_2:
      m_julia = std::complex<double>(-0.4, 0.6);
    break;
    
    case FRACTAL_JULIASET_3:
      m_julia = std::complex<double>(0.285, 0);
    break;
    
    case FRACTAL_JULIASET_4:
      m_julia = std::complex<double>(0.285, 0.01);
    break;
    
    case FRACTAL_JULIASET_5:
      m_julia = std::complex<double>(0.45, 0.1428);
    break;
    
    case FRACTAL_JULIASET_6:
      m_julia = std::complex<double>(-0.70176, -0.3842);
    break;
    
    case FRACTAL_JULIASET_7:
      m_julia = std::complex<double>(-0.835, -0.2321);
    break;
    
    case FRACTAL_JULIASET_8:
      m_julia = std::complex<double>(-0.8, 0.156);
    break;
    
    case FRACTAL_JULIASET_9:
      m_julia = std::complex<double>(0, 1);
    break;
    
    case FRACTAL_GLYNN:
      m_julia = std::complex<double>(-0.2, 0);
    break;
  }
}

bool Fractal::calc(const std::complex<double> & c, uint& n, uint max)
{ 
  if (m_name.contains("glynn"))
  {
    return julia(c, 1.5, n, max);
  } 
  else if (m_name == "julia set")
  {
    return julia(c, m_julia_exponent, n, max);
  }
  else if (m_name.contains("julia"))
  {
    return julia(c, 2, n, max);
  }
  else if (m_name.contains("mandelbrot"))
  {
    return mandelbrotset(c, n, max);
  }
  
  return false;
}          

bool Fractal::julia(const std::complex<double> & c, double exp, uint& n, uint max)
{
  std::complex<double> z(c);
    
  for (n = 0; n < max && !m_thread->interrupted(); n++)
  {
    z = pow(z, exp) + m_julia;
        
    if (abs(z) > m_diverge)
    {
      break;
    }
  }
  
  return !m_thread->interrupted();
}

bool Fractal::mandelbrotset(const std::complex<double> & c, uint& n, uint max)
{
  std::complex<double> z;
    
  for (n = 0; n < max && !m_thread->interrupted(); n++)
  {
    z = z * z - c;
        
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
    m_names.push_back("julia set");
    m_names.push_back("julia set 1");
    m_names.push_back("julia set 2");
    m_names.push_back("julia set 3");
    m_names.push_back("julia set 4");
    m_names.push_back("julia set 5");
    m_names.push_back("julia set 6");
    m_names.push_back("julia set 7");
    m_names.push_back("julia set 8");
    m_names.push_back("julia set 9");
    m_names.push_back("glynn");
  }

  return m_names;  
}
