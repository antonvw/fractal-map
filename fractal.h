#ifndef FRACTAL_H
#define FRACTAL_H

#include <QPixmap>
#include <QLineEdit>
#include <QSpinBox>
#include <QPointF>
#include <QWidget>
#include <QMainWindow>
#include "thread.h"

// This class offers the fractal widget.
class Fractal : public QWidget
{
  Q_OBJECT

public:
  // Constructor.
  Fractal(
    // mainwindow (not NULL)
    QMainWindow* mainwindow,
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
    
  // Access to controls.
  QLineEdit* getCenter() {return m_centerEdit;};
  QSpinBox* getColours() {return m_coloursEdit;};
  QLineEdit* getDiverge() {return m_divergeEdit;};
  QSpinBox* getFirstPass() {return m_first_passEdit;};
  QSpinBox* getPasses() {return m_passesEdit;};
  QLineEdit* getScale(){ return m_scaleEdit;};
  
  // Access to values.
  const QPointF& center() const {return m_center;};
  uint colours() const {return m_colours.size();}
  double diverge() const {return m_diverge;};
  uint firstPass() const {return m_first_pass;}
  uint pass() const {return m_pass;}
  uint passes() const {return m_passes;}
  double scale() const {return m_scale;};
  
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
  void editedCenter(const QString& text);
  void editedColours(int value);
  void editedDiverge(const QString& text);
  void editedFirstPass(int value);
  void editedPasses(int value);
  void editedScale(const QString& text);
  void updatePass(uint pass, uint numberOfPasses, uint iterations);
  void updatePixmap(const QImage &image, double scale);
  void zoom(double zoomFactor);
private:
  uint rgbFromWaveLength(double wave);
  void scroll(const QPoint& delta);
  void setColours(uint colours);

  Thread m_thread;
  
  QLineEdit* m_centerEdit;
  QSpinBox* m_coloursEdit;
  QLineEdit* m_divergeEdit;
  QSpinBox* m_first_passEdit;
  QSpinBox* m_passesEdit;
  QLineEdit* m_scaleEdit;
  
  QPixmap m_pixmap;
  
  QPoint m_lastDragPos;
  QPoint m_origin;
  QPoint m_pixmapOffset;
  QPointF m_center;
  
  std::vector<uint> m_colours;
  
  double m_diverge;
  double m_pixmapScale;
  double m_scale;
  
  uint m_first_pass;
  uint m_pass;
  uint m_passes;
  
  long m_updates;
  
  QMainWindow* m_mainWindow;
};
#endif
