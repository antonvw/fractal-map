////////////////////////////////////////////////////////////////////////////////
// Name:      fractalwidget.h
// Purpose:   Declaration of class FractalWidget
// Author:    Anton van Wezenbeek
// Copyright: (c) 2012 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#ifndef _FRACTALWIDGET_H
#define _FRACTALWIDGET_H

#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QPointF>
#include <QProgressBar>
#include <QSpinBox>
#include <QStatusBar>
#include <QToolBar>
#include <QWidget>
#include "fractalrenderer.h"

// This class offers the fractal widget.
class FractalWidget : public QWidget
{
  Q_OBJECT

public:
  // Constructor.
  FractalWidget(
    // parent
    QWidget* parent,
    // statusbar
    QStatusBar* statusbar,
    // fractal to use
    const QString& fractalName,
    // scale to use
    double scale,
    // number of colours
    uint colours,
    // diverge limit
    double diverge,
    // center
    const QPointF& center,
    // first pass
    uint first_pass,
    // number of passes
    uint passes,
    // extra julia args
    double julia_real,
    double julia_imag,
    double julia_exponent);
    
  // Copy constructor.
  FractalWidget(
    // other fractal widget
    const FractalWidget& fractal,
    // statusbar
    QStatusBar* statusbar);
    
  // Adds controls to a toolbar.
  void addControls(QToolBar* toolbar);

  // Adds julia specific controls to a toolbar.
  void addJuliaControls(QToolBar* toolbar);
  
  // Access to renderer.
  FractalRenderer* renderer() {return &m_renderer;};
public slots:
  // Copies pixmap to clipboard.
  void copy();
  
  // Saves settings.
  void save();
  
  // Sets colours.
  void setColoursDialogBegin() {setColoursDialog(true);};
  void setColoursDialogEnd() {setColoursDialog(false);};
protected:
  void keyPressEvent(QKeyEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void paintEvent(QPaintEvent *event);
  void resizeEvent(QResizeEvent *event);
  void wheelEvent(QWheelEvent *event);
private slots:
  void setAxes(bool state);
  void setCenter(const QString& text);
  void setColoursMax(int value);
  void setColoursMinWave(int value);
  void setColoursMaxWave(int value);
  void setColourSelected(const QColor& color);
  void setDiverge(const QString& text);
  void setFirstPass(int value);
  void setFractal(const QString& index);
  void setJulia(const QString& text);
  void setJuliaExponent(const QString& text);
  void setPasses(int value);
  void setScale(const QString& text);
  void updatePass(uint line, uint max);
  void updatePass(uint pass, uint numberOfPasses, uint iterations);
  void updatePixmap(
    const QImage &image, double scale, int state);
  void zoom(double zoomFactor);
private:
  void addAxes(QPainter& painter);
  void init();
  void render(int start_at = 0);
  void scroll(const QPoint& delta);
  void setColours(uint colours);
  void setColoursDialog(bool from_start);
  uint wav2RGB(double wave) const;

  FractalRenderer m_renderer;
  
  QCheckBox* m_axesEdit;
  QLineEdit* m_centerEdit;
  QSpinBox* m_coloursEdit;
  QSpinBox* m_coloursMaxWaveEdit;
  QSpinBox* m_coloursMinWaveEdit;
  QLineEdit* m_divergeEdit;
  QSpinBox* m_firstPassEdit;
  QComboBox* m_fractalEdit;
  QLineEdit* m_juliaEdit;
  QLineEdit* m_juliaExponentEdit;
  QSpinBox* m_passesEdit;
  QLineEdit* m_scaleEdit;
  
  QLabel* m_passesLabel;
  QLabel* m_updatesLabel;
  
  QPixmap m_pixmap;
  
  QPoint m_lastDragPos;
  QPoint m_origin;
  QPoint m_pixmapOffset;
  QPointF m_center;
  
  std::vector<uint> m_colours;
  
  QString m_fractalName;

  double m_coloursMinWave;  
  double m_coloursMaxWave;  
  double m_diverge;
  double m_juliaExponent;  
  double m_pixmapScale;
  double m_scale;
  
  std::complex<double> m_julia;
    
  uint m_colourIndex;
  uint m_firstPass;
  uint m_pass;
  uint m_passes;
  uint m_updates;
  
  bool m_colourIndexFromStart;
  
  QColorDialog* m_colourDialog;
  QToolBar* m_juliaToolBar;
  QProgressBar* m_progressBar;
  QStatusBar* m_statusBar;
};
#endif
