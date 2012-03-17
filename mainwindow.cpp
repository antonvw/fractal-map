#include <math.h>
#include <QtGui>
#include "mainwindow.h"
#include "fractal.h"

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
{
  qRegisterMetaType<QImage>("QImage");
  
  setCentralWidget(newFractalWidget(parent));
  
  setWindowTitle("Fractal Art");
  
  resize(550, 400);
}

void MainWindow::about()
{
  QMessageBox::about(this, tr("About Menu"),
    "The <b>Menu</b> example shows how to create "
       "menu-bar menus and context menus.");
}

void MainWindow::newFractal()
{
  QWidget* w = newFractalWidget(this);
  w->show();
//  QDockWidget* dock = new QDockWidget(this);
//  dock->setWidget(newFractalWidget(this));
//  dock->show();
//  addDockWidget(Qt::RightDockWidgetArea, dock);
}

QWidget* MainWindow::newFractalWidget(QWidget* parent)
{
  Fractal* fractal = new Fractal(
    this,
    this,
    0.004,
    1024,
    2.0,
    QPointF(0,0),
    8);
    
  QPushButton* newFractal = new QPushButton("new");
    
  connect(newFractal, SIGNAL(clicked()), this, SLOT(newFractal()));
    
  QGridLayout *layout = new QGridLayout;
  
  layout->addWidget(fractal, 0, 0, 1, -1);
  layout->addWidget(fractal->getColours(), 1, 0);
  layout->addWidget(fractal->getPasses(), 1, 1);
  layout->addWidget(fractal->getCenter(), 1, 2);
  layout->addWidget(fractal->getScale(), 1, 3);
  layout->addWidget(fractal->getDiverge(), 1, 4);
  layout->addWidget(newFractal, 1, 5);

  QWidget* widget = new QWidget(parent);
  widget->setLayout(layout);
  
  QWidget* fractalAndControl = new QWidget(this);
  fractalAndControl->setLayout(layout);
  
  fractal->start();
  
  return fractalAndControl;
}
