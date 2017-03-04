////////////////////////////////////////////////////////////////////////////////
// Name:      fractalwidget.h
// Purpose:   Declaration of class FractalWidget
// Author:    Anton van Wezenbeek
// Copyright: (c) 2017 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QProgressBar>
#include <QStatusBar>
#include <QToolBar>
#include <QWidget>
#include <qwt_interval.h>
#include <qwt_plot.h>
#include "fractal.h"
#include "fractalcontrol.h"
#include "fractalrenderer.h"

class PlotZoomer;
class QwtPlotGrid;

/// This class offers the fractal widget.
class FractalWidget : public QwtPlot, public Fractal
{
  Q_OBJECT

public:
  /// Constructor.
  FractalWidget(
    /// parent
    QWidget* parent,
    /// statusbar
    QStatusBar* statusbar,
    /// fractal to use
    const QString& fractalName,
    /// number of colours
    int colours,
    /// images dir
    const QString& dir,
    /// diverge limit
    double diverge,
    /// using this x interval
    const QwtInterval& xInterval,
    /// using this y interval
    const QwtInterval& yInterval,
    /// first pass
    int depth,
    /// julia arg real component
    double julia_real,
    /// julia imag component
    double julia_imag,
    /// julia exponent
    double julia_exponent,
    /// shows axes
    bool show_axes);
    
  /// Copy constructor.
  FractalWidget(
    /// other fractal widget
    const FractalWidget& fractal,
    /// statusbar
    QStatusBar* statusbar);
    
  /// Adds controls to a toolbar.
  void addControls(QToolBar* toolbar);

  /// Adds geometry specific controls to a toolbar.
  void addGeometryControls(QToolBar* toolbar);
  
  /// Adds julia specific controls to a toolbar.
  void addJuliaControls(QToolBar* toolbar);
  
  /// Access to fractal control.
  const auto & fractalControl() const {return m_fractalControl;};
  
  /// Access to fractal pixmap.
  const auto & fractalPixmap() const {return m_fractalPixmap;};
  
  /// Access to renderer.
  auto * renderer() {return &m_fractalRenderer;};
  
public slots:
  /// Zooms in a number of times.
  void autoZoom();
  
  /// Stops auto zooming.
  void autoZoomStop();
  
  /// Copies pixmap to clipboard.
  void copy();
  
  /// Double clicked.
  bool doubleClicked();
 
  /// Saves settings.
  void save();
  
  /// Zooms in.
  void zoomIn() {zoom(0.9);};

  /// Zooms out.
  void zoomOut() {zoom(1.1);};
protected:
  /// Handles resize event.
  virtual void resizeEvent(QResizeEvent *event) override;
private slots:
  /// Renders (starts with) fractal pixmap.
  void render();
  void setAxes(int state);
  void setDiverge(const QString& text);
  void setFractal(const QString& index);
  void setIntervals();
  void setJulia();
  void setJuliaExponent(const QString& text);
  void setSize();
  void updatePixmap(const QImage image, int state);
  void updateProgress(int line, int max);
  void zoomed();
private:
  void init(bool show_axes);
  void zoom(double factor);

  FractalControl m_fractalControl;
  FractalRenderer m_fractalRenderer;
  QPixmap m_fractalPixmap = QPixmap(100, 100);
  
  QCheckBox* m_axesEdit;
  QComboBox* m_fractalEdit;
  QLabel *m_updatesLabel;
  QLineEdit *m_divergeEdit, *m_juliaEdit, *m_juliaExponentEdit, *m_sizeEdit;
  QwtPlotGrid* m_grid;
  PlotZoomer* m_zoom;

  int m_autoZoom = -1;
  int m_updates = 0;
  
  QToolBar* m_juliaToolBar = nullptr;
  QProgressBar* m_progressBar;
  QStatusBar* m_statusBar;
  QToolBar* m_toolBar = nullptr;
};
