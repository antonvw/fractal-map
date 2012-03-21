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
  MainWindow(Fractal* fractal = NULL, QWidget *parent = 0);
private slots:
  void about();
  void colours();
  void copy();
  void newFractal();
  void stop();
private:
  Fractal* m_fractal;
};
#endif
