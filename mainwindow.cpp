#include <math.h>
#include <QtGui>
#include "mainwindow.h"
#include "fractal.h"

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
  , m_fractal(NULL)
{
  qRegisterMetaType<QImage>("QImage");
  
  setCentralWidget(newFractalWidget(this));
  
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
  QMainWindow* m = new QMainWindow(this);
  m->show();
  m->setCentralWidget(newFractalWidget(m));
  m->resize(550, 400);
}

QWidget* MainWindow::newFractalWidget(QMainWindow* parent)
{
  if (m_fractal != NULL)
  {
    m_fractal = new Fractal(
      parent,
      parent,
      m_fractal->scale(),
      m_fractal->colours(),
      m_fractal->diverge(),
      m_fractal->center(),
      m_fractal->first_pass(),
      m_fractal->passes());
  }
  else
  {
    m_fractal = new Fractal(
      parent,
      parent,
      0.004,
      1024,
      2.0,
      QPointF(0,0),
      0,
      8);
  }
    
  QPushButton* newFractal = new QPushButton("new");
    
  connect(newFractal, SIGNAL(clicked()), this, SLOT(newFractal()));
    
  QGridLayout *layout = new QGridLayout;
  
  layout->addWidget(m_fractal, 0, 0, 1, -1);
  layout->addWidget(m_fractal->getColours(), 1, 0);
  layout->addWidget(m_fractal->getFirstPass(), 1, 1);
  layout->addWidget(m_fractal->getPasses(), 1, 2);
  layout->addWidget(m_fractal->getCenter(), 1, 3);
  layout->addWidget(m_fractal->getScale(), 1, 4);
  layout->addWidget(m_fractal->getDiverge(), 1, 5);
  layout->addWidget(newFractal, 1, 6);

  QWidget* widget = new QWidget(parent);
  widget->setLayout(layout);
  
  QWidget* fractalAndControl = new QWidget(this);
  fractalAndControl->setLayout(layout);
  
  m_fractal->start();
  
  return fractalAndControl;
}
