////////////////////////////////////////////////////////////////////////////////
// Name:      fractal.cpp
// Purpose:   Implementation of class Fractal
// Author:    Anton van Wezenbeek
// Copyright: (c) 2012 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

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

std::vector<std::string> Fractal::m_names;

Fractal::Fractal(
  const std::string& name,
  Thread* thread, 
  unsigned int diverge,
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
  
  for (unsigned int i = 0; i < m_names.size() && !m_isOk; i++)
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

bool Fractal::calc(
  const std::complex<double> & c, 
  unsigned int& n, 
  unsigned int max) const
{ 
  if (m_name.find("glynn") != std::string::npos)
  {
    return juliaset(c, 1.5, n, max);
  } 
  else if (m_name == "julia set")
  {
    return juliaset(c, m_julia_exponent, n, max);
  }
  else if (m_name.find("julia") != std::string::npos)
  {
    return juliaset(c, 2, n, max);
  }
  else if (m_name.find("mandelbrot") != std::string::npos)
  {
    return mandelbrotset(c, n, max);
  }
  
  return false;
}          

bool Fractal::juliaset(
  const std::complex<double> & c, double exp, 
  unsigned int& n, 
  unsigned int max) const
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

bool Fractal::mandelbrotset(
  const std::complex<double> & c, 
  unsigned int& n, 
  unsigned int max) const
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

std::vector<std::string> & Fractal::names()
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
