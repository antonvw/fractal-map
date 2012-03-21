#include <math.h>
#include <QtGui>
#include "mainwindow.h"
#include "fractal.h"

MainWindow::MainWindow(Fractal* fractal, QWidget* parent)
  : QMainWindow(parent)
{
  if (fractal != NULL)
  {
    m_fractal = new Fractal(statusBar(), *fractal);
  }
  else
  {
    m_fractal = new Fractal(
      statusBar(),
      this,
      0.004,
      1024,
      2.0,
      QPointF(0,0),
      1,
      8);
  }
    
  qRegisterMetaType<QImage>("QImage");
  
  QPushButton* menuButton = new QPushButton();
  
  QMenu *menu = new QMenu(this);
  QAction* about = menu->addAction("About");
  QAction* colours = menu->addAction("Colours...");
  QAction* copy = menu->addAction("Copy");
  QAction* newFractal = menu->addAction("New");
  QAction* stop = menu->addAction("Stop");
  menuButton->setMenu(menu);
  
  connect(about, SIGNAL(triggered()), this, SLOT(about()));    
  connect(colours, SIGNAL(triggered()), this, SLOT(colours()));
  connect(copy, SIGNAL(triggered()), this, SLOT(copy()));
  connect(newFractal, SIGNAL(triggered()), this, SLOT(newFractal()));
  connect(stop, SIGNAL(triggered()), this, SLOT(stop()));
  connect(menu, SIGNAL(clicked()), this, SLOT(menu()));
  
  QToolBar* tb = addToolBar("Control");
  m_fractal->addControls(tb);
  tb->addWidget(menuButton);
  
  m_fractal->start();
  
  setCentralWidget(m_fractal);
  setWindowTitle("Fractal Map");
  
  resize(550, 400);
}

void MainWindow::about()
{
  QMessageBox::about(this, 
    "About " + windowTitle(),
    "This application shows a fractal map.");
}

void MainWindow::colours()
{
  m_fractal->setColours();
}

void MainWindow::copy()
{
  QApplication::clipboard()->setImage(m_fractal->pixmap().toImage());
}

void MainWindow::newFractal()
{
  MainWindow* m = new MainWindow(m_fractal);
  m->show();
}

void MainWindow::stop()
{
  m_fractal->stop();
}
