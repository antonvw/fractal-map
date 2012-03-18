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
    // pass to start with
    uint first_pass,
    // using max number of passes
    uint passes,
    // using these colours,
    // the last colour is used for converge
    const std::vector<uint> & colours,
    // value that assumes function is diverging
    const double diverge);

  // Stops the thread.
  void stop();
signals:
  // If an image is available, this signal is emitted.
  void renderedImage(const QImage &image, double scale);
  
  // During rendering, this signal is emitted,
  // allowing you to observe progress.
  void renderingImage(uint pass, uint total, uint iterations);
protected:
  void run();
private:
  QMutex m_mutex;
  QWaitCondition m_condition;
  QPointF m_center;
  double m_scale;
  uint m_first_pass;
  uint m_max_passes;
  double m_diverge;
  QSize m_size;
  bool m_restart;
  bool m_stop;
  
  std::vector<uint> m_colours;
};
#endif
