#include <math.h>
#include <QtGui>
#include "mainwindow.h"
#include "fractalwidget.h"

MainWindow::MainWindow(FractalWidget* fractal, QWidget* parent)
  : QMainWindow(parent)
{
  if (fractal != NULL)
  {
    m_fractal = new FractalWidget(*fractal, statusBar());
  }
  else
  {
    m_fractal = new FractalWidget(
      this,
      statusBar(),
      0.007,
      128,
      2.0,
      QPointF(0,0),
      1,
      8);
  }
    
  qRegisterMetaType<QImage>("QImage");
  
  QPushButton* menuButton = new QPushButton();
  menuButton->setFixedWidth(20);
  QMenu *menu = new QMenu(this);
  QAction* about = menu->addAction("About");
  menu->addSeparator();
  QAction* colours_begin = menu->addAction("Colours From Begin...");
  QAction* colours_end = menu->addAction("Colours From End...");
  menu->addSeparator();
  QAction* copy = menu->addAction("Copy");
  menu->addSeparator();
  QAction* newFractalWidget = menu->addAction("New");
  menu->addSeparator();
  QAction* pause = new QAction("Pause", menu);
  pause->setCheckable(true);
  menu->addAction(pause);
  menuButton->setMenu(menu);
  
  connect(about, SIGNAL(triggered()), this, SLOT(about()));    
  connect(colours_begin, SIGNAL(triggered()), this, SLOT(colours_begin()));
  connect(colours_end, SIGNAL(triggered()), this, SLOT(colours_end()));
  connect(copy, SIGNAL(triggered()), this, SLOT(copy()));
  connect(newFractalWidget, SIGNAL(triggered()), this, SLOT(newFractalWidget()));
  connect(pause, SIGNAL(toggled(bool)), this, SLOT(pause(bool)));
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
    QString("This application shows a fractal map.\nBuilt using Qt %1").
      arg(QT_VERSION_STR));
}

void MainWindow::colours_begin()
{
  m_fractal->setColoursDialog();
}

void MainWindow::colours_end()
{
  m_fractal->setColoursDialog(false);
}

void MainWindow::copy()
{
  QApplication::clipboard()->setImage(m_fractal->pixmap().toImage());
}

void MainWindow::newFractalWidget()
{
  MainWindow* m = new MainWindow(m_fractal);
  m->show();
}

void MainWindow::pause(bool checked)
{
  checked ? m_fractal->pause(): m_fractal->cont();
}
