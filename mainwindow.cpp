#include <math.h>
#include <QtGui>
#include "mainwindow.h"
#include "fractal.h"

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
{
  Fractal* fractal = new Fractal(this);
  
  setCentralWidget(fractal);
  setWindowTitle("Fractal Art");
  
  QDockWidget* dock = new QDockWidget(this);
  QWidget* control = fractal->addControl(dock);
  dock->setWidget(control);
  addDockWidget(Qt::LeftDockWidgetArea, dock);
  
  resize(550, 400);
}
