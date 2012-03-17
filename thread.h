#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

#include <vector>
#include <QMutex>
#include <QSize>
#include <QImage>
#include <QThread>
#include <QPointF>
#include <QWaitCondition>

// This class offers a thread to render the fractal image.
class Thread : public QThread
{
  Q_OBJECT

public:
  // Constructor.
  Thread(QObject* parent = 0);
  
  // Destructor, takes running of finishing the thread main loop.
 ~Thread();

  // Begins rendering the image.
  void render(
    // using this center
    const QPointF& center,
    // using this scale
    double scale,
    // using this size
    const QSize& size,
    // using max number of passes
    uint passes,
    // using these colours,
    // the last colour is used for converge
    const std::vector<uint> & colours,
    // value that assumes function is diverging
    const double diverge);
    
signals:
  // If an image is available, this signal is emitted.
  void renderedImage(const QImage &image, double scale);
  
  // During rendering, this signal is emitted,
  // allowing you to observe progress.
  void renderingImage(int pass, int total, int iterations);
protected:
  void run();
private:
  QMutex m_mutex;
  QWaitCondition m_condition;
  QPointF m_center;
  double m_scale;
  uint m_passes;
  double m_diverge;
  QSize m_size;
  bool m_restart;
  bool m_stop;
  
  std::vector<uint> m_colours;
};
#endif
