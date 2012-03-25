#ifndef FRACTALWIDGET_H
#define FRACTALWIDGET_H

#include <QPixmap>
#include <QLineEdit>
#include <QSpinBox>
#include <QPointF>
#include <QWidget>
#include <QColorDialog>
#include <QComboBox>
#include <QStatusBar>
#include <QCheckBox>
#include <QToolBar>
#include "thread.h"

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
    uint passes);
    
  // Copy constructor.
  FractalWidget(
    // other fractal widget
    const FractalWidget& fractal,
    // statusbar
    QStatusBar* statusbar);
    
  // Adds controls to a toolbar.
  void addControls(QToolBar* toolbar);

  // Continues thread.
  void cont() {m_thread.cont();}
  
  // Gets current pixmap.  
  const QPixmap& pixmap() const {return m_pixmap;};
  
  // Sets colours.
  void setColoursDialog(bool from_start = true);
  
  // Pauses thread.
  void pause() {m_thread.pause();}
  
  // Refreshes thread.
  void refresh() {m_thread.refresh();}
  
  // Starts thread.
  void start() {m_thread.start();}
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
  void setPasses(int value);
  void setScale(const QString& text);
  void updatePass(uint pass, uint numberOfPasses, uint iterations);
  void updatePixmap(const QImage &image, double scale);
  void zoom(double zoomFactor);
private:
  void addAxes(QPainter& painter);
  void init();
  void render(int start_at = 0);
  void scroll(const QPoint& delta);
  void setColours(uint colours);
  uint wav2RGB(double wave) const;

  Thread m_thread;
  
  QCheckBox* m_axesEdit;
  QLineEdit* m_centerEdit;
  QSpinBox* m_coloursEdit;
  QSpinBox* m_coloursMaxWaveEdit;
  QSpinBox* m_coloursMinWaveEdit;
  QLineEdit* m_divergeEdit;
  QSpinBox* m_first_passEdit;
  QComboBox* m_fractalEdit;
  QSpinBox* m_passesEdit;
  QLineEdit* m_scaleEdit;
  
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
  double m_pixmapScale;
  double m_scale;
  
  std::complex<double> m_julia;
    
  uint m_colourIndex;
  uint m_first_pass;
  uint m_pass;
  uint m_passes;
  
  long m_updates;
  
  bool m_colourIndexFromStart;
  
  QColorDialog* m_colourDialog;
  QStatusBar* m_statusbar;
};
#endif
