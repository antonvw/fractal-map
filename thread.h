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

enum RenderingState
{
  RENDERING_INIT,
  RENDERING_IN_PROGRESS,
  RENDERING_PAUSED,
  RENDERING_READY,
  RENDERING_RESTART,
  RENDERING_SKIP,
  RENDERING_SNAPSHOT,
  RENDERING_STOPPED,
};

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
    return 
      m_state == RENDERING_PAUSED || 
      m_state == RENDERING_RESTART || 
      m_state == RENDERING_SKIP || 
      m_state == RENDERING_SNAPSHOT || 
      m_state == RENDERING_STOPPED;};
  
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
public slots:
  // Pauses or continues the thread.
  void pause(bool checked);
  
  // Ask for a refresh.
  void refresh();
    
  // Skip current pass.
  void skip();
signals:
  // If an image is available, this signal is emitted.
  void renderedImage(
    const QImage &image, 
    double scale,
    int state);
  
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
    uint max, 
    QImage& image, 
    const QPoint& p, 
    int inc,
    const std::vector<uint> & colours);
  // End current state.
  bool end() const;
  // Stops the thread (finishes the thread main loop).
  void stop();
  
  QImage m_image;
  QMutex m_mutex;
  QWaitCondition m_condition;
  QPointF m_center;
  
  double m_scale;
  
  uint m_first_pass;
  uint m_max_passes;
  
  int m_state;
  
  Fractal m_fractal;
  
  std::vector<uint> m_colours;
};
#endif
