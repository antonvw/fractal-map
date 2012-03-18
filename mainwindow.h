#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Fractal;

// This class offers the main window.
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  // Constructor.
  MainWindow(QWidget *parent = 0);
private slots:
  void about();
  void newFractal();
  void stop();
private:
  QWidget* newFractalWidget(QMainWindow* parent);
  
  Fractal* m_fractal;
};
#endif
