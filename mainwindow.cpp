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
  QToolBar* tb = parent->addToolBar("Control");

  if (m_fractal != NULL)
  {
    m_fractal = new Fractal(
      parent,
      parent,
      m_fractal->scale(),
      m_fractal->colours(),
      m_fractal->diverge(),
      m_fractal->center(),
      m_fractal->pass(),
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
  QPushButton* stop = new QPushButton("stop");
    
  connect(newFractal, SIGNAL(clicked()), this, SLOT(newFractal()));
  connect(stop, SIGNAL(clicked()), this, SLOT(stop()));
  
  tb->addWidget(m_fractal->getColours());
  tb->addWidget(m_fractal->getFirstPass());
  tb->addWidget(m_fractal->getPasses());
  tb->addWidget(m_fractal->getCenter());
  tb->addWidget(m_fractal->getScale());
  tb->addWidget(m_fractal->getDiverge());
  tb->addWidget(stop);
  tb->addWidget(newFractal);
  
  m_fractal->start();

  return m_fractal;
}

void MainWindow::stop()
{
  m_fractal->stop();
}
