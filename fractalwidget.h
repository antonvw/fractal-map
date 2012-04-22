////////////////////////////////////////////////////////////////////////////////
// Name:      fractalwidget.h
// Purpose:   Declaration of class FractalWidget
// Author:    Anton van Wezenbeek
// Copyright: (c) 2012 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#ifndef _FRACTALWIDGET_H
#define _FRACTALWIDGET_H

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QPointF>
#include <QProgressBar>
#include <QSpinBox>
#include <QStatusBar>
#include <QTime>
#include <QToolBar>
#include <QWidget>
#include <qwt_plot.h>
#include "fractalrenderer.h"
#include "fractal.h"
#include "fractalgeometry.h"

class QwtPlotMagnifier;

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
    /// scale to use
    double scale,
    /// number of colours
    int colours,
    /// images dir
    const QString& dir,
    /// diverge limit
    double diverge,
    /// center
    const QPointF& center,
    /// first pass
    int first_pass,
    /// number of passes
    int passes,
    /// julia arg real component
    double julia_real,
    /// julia imag component
    double julia_imag,
    /// julia exponent
    double julia_exponent);
    
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
  
  /// Access to geometry.
  FractalGeometry* geometry() {return &m_fractalGeo;};
  
  /// Access to renderer.
  FractalRenderer* renderer() {return &m_fractalRenderer;};
  
  const QPixmap& fractalPixmap() const {return m_fractalPixmap;};
  
public slots:
  /// Copies pixmap to clipboard.
  void copy();
  
  /// Saves settings.
  void save();
protected:
  /// Handles key press event.
  void keyPressEvent(QKeyEvent *event);
  
  /// Handles resize event.
  void resizeEvent(QResizeEvent *event);

  /// Handles key press event.
  void wheelEvent(QWheelEvent *event);
private slots:
  void render();
  void setAxes(int state);
  void setDiverge(const QString& text);
  void setFractal(const QString& index);
  void setJulia();
  void setJuliaExponent(const QString& text);
  void setSize();
  void updatePass(int line, int max);
  void updatePass(int pass, int numberOfPasses, int iterations);
  void updatePixmap(
    const QImage &image, double scale, int state);
private:
  void init();

  FractalGeometry m_fractalGeo;
  QPixmap m_fractalPixmap;
  FractalRenderer m_fractalRenderer;
  
  QCheckBox* m_axesEdit;
  QLineEdit* m_divergeEdit;
  QComboBox* m_fractalEdit;
  QLineEdit* m_juliaEdit;
  QLineEdit* m_juliaExponentEdit;
  QLineEdit* m_sizeEdit;
  
  QLabel* m_maxPassesLabel;
  QLabel* m_updatesLabel;
  
  QwtPlotMagnifier* m_plotMagnifier;
  
  QTime m_time;
  
  int m_updates;
  
  QToolBar* m_juliaToolBar;
  QProgressBar* m_progressBar;
  QStatusBar* m_statusBar;
  QToolBar* m_toolBar;
};
#endif
