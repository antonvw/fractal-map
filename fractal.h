#ifndef FRACTAL_H
#define FRACTAL_H

#include <complex>
#include <vector>
#include <QString>

class Thread;

// This class offers a fractal.
class Fractal
{
public:
  // Constructor.
  Fractal(
    // The thread, to be able to interrupt calculation.
    Thread* thread, 
    // The name of the fractal, see names.
    const QString& name,
    // Diverge limit.
    uint diverge);

  // Do fractal calculation. 
  bool calc(double ax, double ay, uint& n, uint max);
  
  // Is name ok.
  bool isOk() const;
  
  // Supported fractals.
  static std::vector<QString> & names();
private:
  bool julia(
    const std::complex<double> & c, double ax, double ay, uint& n, uint max);
  
  static std::vector<QString> m_names;
  
  bool m_isOk;
  uint m_diverge;
  int m_type;
  const QString m_name;
  Thread* m_thread;
};
#endif
