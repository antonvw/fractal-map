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

class FractalRenderer;

/// This class offers fractal calculations.
class Fractal
{
public:
  /// Default constructor.
  Fractal(
    /// the name of the fractal, see names
    const std::string& name = std::string(),
    /// diverge limit
    double diverge = 2,
    /// complex arg (for julia set)
    const std::complex<double> & c = std::complex<double>(0, 0),
    /// exponent (for julia set)
    double exp = 2);
    
  /// Do fractal calculation. 
  /// Returns true if calculation was not interrupted by renderer.
  bool calc(
    /// complex start value
    const std::complex<double> & c,
    /// number of iterations before diverge
    int& n, 
    /// max iterations
    int max) const;
    
  /// Is this fractal ok?
  bool isOk() const;
  
  /// Gets the name.
  const std::string& name() const {return m_name;};
  
  /// Update name.
  bool setName(const std::string& name);

  /// Sets the renderer.
  void setRenderer(FractalRenderer* renderer) {m_renderer = renderer;};
    
  /// Supported fractals.
  static std::vector<std::string> & names();
protected:
  double m_diverge;
  std::complex<double> m_julia;
  double m_juliaExponent;
private:  
  bool juliaset(
    const std::complex<double> & c, 
    double exp, 
    int& n, 
    int max) const;
  bool mandelbrotset(
    const std::complex<double> & c, 
    int& n, 
    int max) const;
  
  FractalRenderer* m_renderer;
  std::string m_name;
  static std::vector<std::string> m_names;
};
#endif
