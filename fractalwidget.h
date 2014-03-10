////////////////////////////////////////////////////////////////////////////////
// Name:      fractalwidget.h
// Purpose:   Declaration of class FractalWidget
// Author:    Anton van Wezenbeek
// Copyright: (c) 2014 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#ifndef _FRACTALWIDGET_H
#define _FRACTALWIDGET_H

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QProgressBar>
#include <QStatusBar>
#include <QTime>
#include <QToolBar>
#include <QWidget>
#include <qwt_interval.h>
#include <qwt_plot.h>
#include "fractal.h"
#include "fractalgeometry.h"
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
    int first_pass,
    /// number of passes
    int passes,
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

  /// Adds julia specific controls to a toolbar.
  void addJuliaControls(QToolBar* toolbar);
  
  /// Access to fractal pixmap.
  const QPixmap& fractalPixmap() const {return m_fractalPixmap;};
  
  /// Access to geometry.
  const FractalGeometry& geometry() const {return m_fractalGeo;};
  
  /// Access to renderer.
  FractalRenderer* renderer() {return &m_fractalRenderer;};
  
public slots:
  /// Zooms in a number of times.
  void autoZoom();
  
  /// Copies pixmap to clipboard.
  void copy();
  
  /// Double clicked.
  bool doubleClicked();
  
  /// Saves settings.
  void save();
  
  /// Zooms in.
  void zoom();
protected:
  /// Handles resize event.
  void resizeEvent(QResizeEvent *event);
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
  void updatePass(int line, int max);
  void updatePass(int pass, int numberOfPasses, int iterations);
  void updatePixmap(const QImage &image, int state);
  void zoomed();
private:
  void init(bool show_axes);

  FractalGeometry m_fractalGeo;
  QPixmap m_fractalPixmap;
  FractalRenderer m_fractalRenderer;
  
  QCheckBox* m_axesEdit;
  QLineEdit* m_divergeEdit;
  QComboBox* m_fractalEdit;
  QLineEdit* m_juliaEdit;
  QLineEdit* m_juliaExponentEdit;
  QLineEdit* m_sizeEdit;
  
  QLabel* m_passesLabel;
  QLabel* m_updatesLabel;
  
  QwtPlotGrid* m_grid;
  PlotZoomer* m_zoom;
  
  QTime m_time;

  int m_autoZoom;
  int m_updates;
  
  QToolBar* m_juliaToolBar;
  QProgressBar* m_progressBar;
  QStatusBar* m_statusBar;
  QToolBar* m_toolBar;
};
#endif
