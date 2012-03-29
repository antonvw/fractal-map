////////////////////////////////////////////////////////////////////////////////
// Name:      fractal.h
// Purpose:   Declaration of class Fractal
// Author:    Anton van Wezenbeek
// Copyright: (c) 2012 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#ifndef _FRACTAL_H
#define _FRACTAL_H

#include <complex>
#include <string>
#include <vector>

class Thread;

// This class offers fractal calculations.
class Fractal
{
public:
  // Default constructor.
  Fractal(
    // the name of the fractal, see names
    const std::string& name = std::string(),
    // the thread, to be able to interrupt calculation
    Thread* thread = NULL, 
    // diverge limit
    unsigned int diverge = 0,
    // complex arg (for julia set)
    const std::complex<double> & c = std::complex<double>(0, 0),
    // exponent (for julia set)
    double exp = 2);
    
  // Do fractal calculation. 
  // Returns true if calculation was not interrupted by thread.
  bool calc(
    // complex start value
    const std::complex<double> & c,
    // number of iterations before diverge
    unsigned int& n, 
    // max iterations
    unsigned int max) const;
  
  // Is name ok.
  bool isOk() const {return m_isOk;};
  
  // Supported fractals.
  static std::vector<std::string> & names();
private:
  bool juliaset(
    const std::complex<double> & c, 
    double exp, 
    unsigned int& n, 
    unsigned int max) const;
  bool mandelbrotset(
    const std::complex<double> & c, 
    unsigned int& n, 
    unsigned int max) const;
  
  static std::vector<std::string> m_names;
  
  bool m_isOk;
  unsigned int m_diverge;
  std::string m_name;
  Thread* m_thread;
  std::complex<double> m_julia;
  double m_julia_exponent;
};
#endif
