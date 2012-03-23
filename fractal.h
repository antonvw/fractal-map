#ifndef FRACTAL_H
#define FRACTAL_H

#include <complex>
#include <vector>
#include <QString>

class Thread;

// This class offers fractal calculations.
class Fractal
{
public:
  // Default constructor.
  Fractal(
    // the thread, to be able to interrupt calculation
    Thread* thread = NULL, 
    // the name of the fractal, see names
    const QString& name = QString(),
    // diverge limit
    uint diverge = 0,
    // extra arg (for julia set)
    const std::complex<double> & c = std::complex<double>(0, 0));
    
  // Do fractal calculation. 
  // Returns true if calculation was not interrupted by thread.
  bool calc(
    // complex start value
    const std::complex<double> & c,
    // number of iterations before diverge
    uint& n, 
    // max iterations
    uint max);
  
  // Is name ok.
  bool isOk() const {return m_isOk;};
  
  // Supported fractals.
  static std::vector<QString> & names();
private:
  bool julia(const std::complex<double> & c, uint& n, uint max);
  
  static std::vector<QString> m_names;
  
  bool m_isOk;
  uint m_diverge;
  QString m_name;
  Thread* m_thread;
  std::complex<double> m_julia;
};
#endif
