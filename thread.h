////////////////////////////////////////////////////////////////////////////////
// Name:      thread.h
// Purpose:   Declaration of class Thread
// Author:    Anton van Wezenbeek
// Copyright: (c) 2012 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#ifndef _THREAD_H
#define _THREAD_H

#include <complex>
#include <vector>
#include <QImage>
#include <QMutex>
#include <QPoint>
#include <QPointF>
#include <QThread>
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
  
  // Destructor, stops rendering.
 ~Thread();
 
  // Thread is interrupted.
  bool interrupted() const {
    return m_pause || m_refresh || m_restart || m_skip || m_stop;};
  
  // Pauses or continues the thread.
  void pause(bool checked);
  
  // Begins rendering the fractal into an image (if the thread is running).
  // Returns false if parameters conflict.
  bool render(
    // using this fractal
    const Fractal& fractal,
    // using this image
    const QImage& image,
    // using this center
    const QPointF& center,
    // using this scale
    double scale,
    // pass to start with
    uint pass,
    // using max number of passes
    uint max,
    // using these colours,
    // the last colour is used for converge
    const std::vector<uint> & colours);
    
  // Ask for a refresh.
  void refresh();
  
  // Skip current pass.
  void skip();
signals:
  // If an image is available, this signal is emitted.
  void renderedImage(
    const QImage &image, 
    bool ready,
    double scale,
    bool snapshot);
  
  // During rendering, this signal is emitted,
  // allowing you to observe progress.
  void renderingImage(uint pass, uint max, uint iterations);
  
  // During rendering, this signal is emitted as well.
  // It signals current busy on line out of max lines.
  void renderingImage(uint line, uint max);
protected:
  void run();
private:
  bool render(
    const Fractal& fractal,
    const std::complex<double> & c, 
    QImage& image, 
    const std::vector<uint> & colours,
    const QPoint& p, 
    uint max, 
    bool& converge);
  // Stops the thread (finishes the thread main loop).
  void stop();
  
  QImage m_image;
  QMutex m_mutex;
  QWaitCondition m_condition;
  QPointF m_center;
  
  double m_scale;
  
  uint m_first_pass;
  uint m_max_passes;
  
  bool m_pause;
  bool m_refresh;
  bool m_restart;
  bool m_skip;
  bool m_stop;
  
  Fractal m_fractal;
  
  std::vector<uint> m_colours;
};
#endif
