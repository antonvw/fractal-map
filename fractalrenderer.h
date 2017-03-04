////////////////////////////////////////////////////////////////////////////////
// Name:      fractalrenderer.h
// Purpose:   Declaration of class FractalRenderer
// Author:    Anton van Wezenbeek
// Copyright: (c) 2017 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <QImage>
#include <QMutex>
#include <QPoint>
#include <QThread>
#include <QWaitCondition>
#include "fractal.h"
#include "fractalgeometry.h"

enum RenderingState
{
  RENDERING_INIT,      /// INIT state
  RENDERING_READY,     /// READY state
  RENDERING_ACTIVE,    /// ACTIVE state
  RENDERING_INTERRUPT, /// INTERRUPT state
  RENDERING_PAUSED,    /// PAUSED state
  RENDERING_STOPPED,   /// STOPPED state
  RENDERING_START,     /// START state
  RENDERING_SNAPSHOT,  /// SNAPSHOT state
};

/// This class renders the fractal image.
/// Just call start to start the process, after which you can render images.
/// \dot
/// digraph RenderingState {
///   node [shape=doublecircle]; INIT; STOPPED;
///   node [shape=circle fixedsize width=1.5 height=1];
///   {rank = same; INIT; STOPPED;}
///   {rank = same; SKIP; SNAPSHOT;}
///   {rank = same; READY; PAUSED;}
///   INIT      -> READY     [ label = "start" ];
///   READY     -> START     [ label = "render" ];
///   READY     -> STOPPED   [ label = "stop" ];
///   READY     -> PAUSED    [ label = "pause" ];
///   START     -> ACTIVE    [ label = "geo = m_geo" ];
///   ACTIVE    -> PAUSED    [ label = "pause" ];
///   ACTIVE    -> SKIP      [ label = "skip" ];
///   ACTIVE    -> INTERRUPT [ label = "interrupt" ];
///   ACTIVE    -> SNAPSHOT  [ label = "refresh" ];
///   ACTIVE    -> STOPPED   [ label = "stop" ];
///   ACTIVE    -> START     [ label = "render" ];
///   ACTIVE    -> READY     [ label = "pass == geo.maxPasses" ];
///   PAUSED    -> ACTIVE    [ label = "cont" ];
///   PAUSED    -> READY     [ label = "cont" ];
///   PAUSED    -> STOPPED   [ label = "stop" ];
///   INTERRUPT -> ACTIVE    [ label = "cont" ];
///   SNAPSHOT  -> ACTIVE;
///  }
/// \enddot
class FractalRenderer : public QThread
{
  Q_OBJECT

public:
  /// Constructor.
  FractalRenderer(QObject* parent = 0);
  
  /// Destructor, stops rendering.
 ~FractalRenderer();
 
  /// Interrupts rendering.
  /// Call render or cont to render again.
  void interrupt();
 
  /// Process is interrupted.
  bool interrupted() const;
public slots:
  /// Pauses or continues rendering.
  void pause(bool checked) {checked ? pause(): cont();};
  
  /// Ask for a refresh, a rendered image will be emitted,
  /// though the image will not be finished.
  void refresh();
    
  /// Begins rendering the fractal into an image (if the process is started).
  /// Returns false if fractal or geometry not ok, or rendering is not allowed.
  bool render(
    /// using this fractal
    const Fractal& fractal,
    /// using this image
    const QImage& image,
    /// using this geometry
    const FractalGeometry& geometry);
    
  /// Restarts rendering.
  void restart();
  
  /// Starts process.
  void start() {QThread::start();};
signals:
  /// If an image is available, this signal is emitted.
  void rendered(const QImage image, int state);
  
  /// During rendering, this signal is emitted.
  /// It signals current busy on line out of max lines.
  void rendering(int line, int max);
protected:
  /// Overriden from base class.
  virtual void run() override;
private:
  const QSize calcStep(const FractalGeometry& geo) const;
  void cont();
  bool nextStateForCalcEnd(const QImage& image);
  void pause();
  bool render(
    const Fractal& fractal,
    const std::complex<double> & c, 
    const FractalGeometry& geo,
    QImage& image, 
    const QPoint& p, 
    const QSize& inc);
  void stop();
  
  QWaitCondition m_condition;
  QImage m_image;
  QMutex m_mutex;
  
  int m_state = RENDERING_INIT;
  int m_oldState = RENDERING_INIT;
  
  Fractal m_fractal;
  FractalGeometry m_geo;
};
