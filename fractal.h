#ifndef FRACTAL_H
#define FRACTAL_H

#include <QPixmap>
#include <QLineEdit>
#include <QSpinBox>
#include <QPointF>
#include <QWidget>
#include <QComboBox>
#include <QStatusBar>
#include <QCheckBox>
#include <QToolBar>
#include "thread.h"

// This class offers the fractal widget.
class Fractal : public QWidget
{
  Q_OBJECT

public:
  // Constructor.
  Fractal(
    // statusbar (not NULL)
    QStatusBar* statusbar,
    // parent (NULL allowed)
    QWidget* parent,
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
  Fractal(
    // statusbar (not NULL)
    QStatusBar* statusbar,
    // other fractal
    const Fractal& fractal);
    
  // Adds controls to a toolbar.
  void addControls(QToolBar* toolbar);

  // Gets current pixmap.  
  const QPixmap& pixmap() const {return m_pixmap;};
  
  // Sets colours.
  void setColours();
  
  // Starts rendering.
  void start();
  
  // Stops rendering.
  void stop();
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
  void setDiverge(const QString& text);
  void setFirstPass(int value);
  void setFractal(const QString& index);
  void setPasses(int value);
  void setMaxColours(int value);
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
  QLineEdit* m_divergeEdit;
  QSpinBox* m_first_passEdit;
  QComboBox* m_fractalEdit;
  QSpinBox* m_passesEdit;
  QSpinBox* m_coloursMinWaveEdit;
  QSpinBox* m_coloursMaxWaveEdit;
  QLineEdit* m_scaleEdit;
  
  QPixmap m_pixmap;
  
  QPoint m_lastDragPos;
  QPoint m_origin;
  QPoint m_pixmapOffset;
  QPointF m_center;
  
  std::vector<uint> m_colours;
  
  QString m_fractalType;
  
  double m_diverge;
  double m_pixmapScale;
  double m_scale;
  
  uint m_first_pass;
  uint m_pass;
  uint m_passes;
  
  long m_updates;
  
  QStatusBar* m_statusbar;
};
#endif
