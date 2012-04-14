////////////////////////////////////////////////////////////////////////////////
// Name:      fractalrenderer.h
// Purpose:   Declaration of class FractalRenderer
// Author:    Anton van Wezenbeek
// Copyright: (c) 2012 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#ifndef _FRACTALRENDERER_H
#define _FRACTALRENDERER_H

#include <complex>
#include <vector>
#include <QImage>
#include <QMutex>
#include <QPoint>
#include <QPointF>
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
  RENDERING_RESET,     /// RESET state
  RENDERING_STOPPED,   /// STOPPED state
  RENDERING_START,     /// START state
  RENDERING_SKIP,      /// SKIP state
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
///   INTERRUPT -> RESET     [ label = "reset" ];
///   RESET     -> START     [ label = "render" ];
///   SKIP      -> ACTIVE;
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
  bool interrupted() const {
    return 
      m_state == RENDERING_PAUSED || 
      m_state == RENDERING_INTERRUPT || 
      m_state == RENDERING_RESET || 
      m_state == RENDERING_START || 
      m_state == RENDERING_SKIP || 
      m_state == RENDERING_SNAPSHOT || 
      m_state == RENDERING_STOPPED;};
      
  /// Resets after interrupt.
  /// Current pass is not finished, no sigal is emitted.
  void reset();
    
public slots:
  /// Continues rendering from where it was interrupted.
  void cont();
  
  /// Pauses rendering.
  /// Calling render after pause will have no effect, until you call cont.
  void pause();
  
  /// Pauses or continues rendering.
  void pause(bool checked) {checked ? pause(): cont();};
  
  /// Ask for a refresh, a rendered image will be emitted,
  /// though the image will not be finished.
  void refresh();
    
  /// Begins rendering the fractal into an image (if the process is started).
  /// Returns false if parameters conflict.
  bool render(
    /// using this fractal
    const Fractal& fractal,
    /// using this image
    const QImage& image,
    /// using this geometry
    const FractalGeometry& geometry);
    
  /// Skips current pass.
  void skip();
  
  /// Starts process.
  void start();
signals:
  /// If an image is available, this signal is emitted.
  void rendered(
    const QImage &image, 
    double scale,
    int state);
  
  /// During rendering, this signal is emitted,
  /// allowing you to observe progress.
  void rendering(int pass, int max, int iterations);
  
  /// During rendering, this signal is emitted as well.
  /// It signals current busy on line out of max lines.
  void rendering(int line, int max);
protected:
  /// Overriden from base class.
  void run();
private:
  int calcStep(int pass, int first_pass, int max_passes) const;
  // End current state.
  bool end() const;
  bool render(
    const Fractal& fractal,
    const std::complex<double> & c, 
    int max, 
    QImage& image, 
    const QPoint& p, 
    int inc,
    const std::vector<uint> & colours);
  // Stops rendering.
  void stop();
  
  QWaitCondition m_condition;
  QImage m_image;
  QMutex m_mutex;
  
  int m_state;
  int m_oldState;
  
  Fractal m_fractal;
  FractalGeometry m_geo;
};
#endif
