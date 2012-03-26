#ifndef XTHREAD_H
#define XTHREAD_H

#include <vector>
#include <QMutex>
#include <QSize>
#include <QImage>
#include <QThread>
#include <QPointF>
#include <QWaitCondition>
#include "fractal.h"

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
 
  // Continues.
  void cont();

  // Thread is paused or restarted or stopped.  
  bool interrupted() const {
    return m_pause || m_refresh || m_restart || m_stop;};
  
  // Pauses the thread.
  void pause();
  
  // Is thread stopped.
  bool paused() const {return m_pause;};

  // Begins rendering the fractal into an image (if the thread is running).
  void render(
    // using this fractal
    const Fractal& fractal,
    // using this image
    const QImage& image,
    // using this center
    const QPointF& center,
    // using this scale
    double scale,
    // pass to start with
    uint first_pass,
    // using max number of passes
    uint passes,
    // using these colours,
    // the last colour is used for converge
    const std::vector<uint> & colours);
    
  // Ask for a refresh.
  void refresh();
signals:
  // If an image is available, this signal is emitted.
  void renderedImage(
    const QImage &image, 
    double scale,
    bool snapshot);
  
  // During rendering, this signal is emitted,
  // allowing you to observe progress.
  void renderingImage(uint pass, uint total, uint iterations);
protected:
  void run();
private:
  // Stops the thread (finishes the thread main loop).
  void stop();
  
  QMutex m_mutex;
  QWaitCondition m_condition;
  QPointF m_center;
  double m_scale;
  uint m_first_pass;
  uint m_max_passes;
  bool m_pause;
  bool m_refresh;
  bool m_restart;
  bool m_stop;
  QImage m_image;
  Fractal m_fractal;
  
  std::vector<uint> m_colours;
};
#endif
