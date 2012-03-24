#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class FractalWidget;

// This class offers the main window.
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  // Constructor.
  MainWindow(FractalWidget* fractal = NULL, QWidget *parent = 0);
private slots:
  void about();
  void colours_begin();
  void colours_end();
  void copy();
  void newFractalWidget();
  void pause(bool);
  void refresh();
private:
  FractalWidget* m_fractalWidget;
};
#endif
