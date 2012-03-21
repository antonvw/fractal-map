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
// Just call start to start the thread, after which you can render images.
class Thread : public QThread
{
  Q_OBJECT

public:
  // Constructor.
  Thread(QObject* parent = 0);
  
  // Destructor, invokes stop.
 ~Thread();

  // Supported fractals.
  static std::vector<QString> & fractals() {return m_fractals;};
  
  // Begins rendering the image (if the thread is running).
  void render(
    // kind of fractal
    const QString& fractal,
    // using this center
    const QPointF& center,
    // using this scale
    double scale,
    // using this image
    const QImage& image,
    // pass to start with
    uint first_pass,
    // using max number of passes
    uint passes,
    // using these colours,
    // the last colour is used for converge
    const std::vector<uint> & colours,
    // value that assumes function is diverging
    const double diverge);

  // Stops the thread (finishes the thread main loop).
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
  bool fractal(double ax, double ay, uint& n, uint max, uint diverge, int type);
  
  QMutex m_mutex;
  QWaitCondition m_condition;
  QPointF m_center;
  double m_scale;
  uint m_first_pass;
  uint m_max_passes;
  double m_diverge;
  bool m_restart;
  bool m_stop;
  QImage m_image;
  int m_fractal;
  
  std::vector<uint> m_colours;
  static std::vector<QString> m_fractals;
};
#endif
