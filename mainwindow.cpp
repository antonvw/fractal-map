#include <math.h>
#include <QtGui>
#include <QSettings>
#include "mainwindow.h"
#include "fractalwidget.h"

MainWindow::MainWindow(FractalWidget* fractal, QWidget* parent)
  : QMainWindow(parent)
{
  if (fractal != NULL)
  {
    m_fractalWidget = new FractalWidget(*fractal, statusBar());
  }
  else
  {
    QSettings settings;
    
    m_fractalWidget = new FractalWidget(
      this,
      statusBar(),
      settings.value("fractal", "julia set 4").toString(),
      settings.value("scale", 0.007).toDouble(),
      settings.value("colours", 128).toInt(),
      2.0,
      QPointF(0,0),
      settings.value("first pass", 1).toInt(),
      settings.value("last pass", 10).toInt());
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
  copy->setShortcuts(QKeySequence::Copy);
  QAction* refresh = menu->addAction("Refresh");
  refresh->setShortcuts(QKeySequence::Refresh);
  menu->addSeparator();
  QAction* newAction = menu->addAction("New");
  newAction->setShortcuts(QKeySequence::New);
  menu->addSeparator();
  QAction* pause = new QAction("Pause", menu);
  pause->setCheckable(true);
  menu->addAction(pause);
  menuButton->setMenu(menu);
  
  connect(about, SIGNAL(triggered()), this, SLOT(about()));    
  connect(colours_begin, SIGNAL(triggered()), this, SLOT(colours_begin()));
  connect(colours_end, SIGNAL(triggered()), this, SLOT(colours_end()));
  connect(copy, SIGNAL(triggered()), this, SLOT(copy()));
  connect(refresh, SIGNAL(triggered()), this, SLOT(refresh()));
  connect(newAction, SIGNAL(triggered()), this, SLOT(newFractalWidget()));
  connect(pause, SIGNAL(toggled(bool)), this, SLOT(pause(bool)));
  connect(menu, SIGNAL(clicked()), this, SLOT(menu()));
  
  QToolBar* tb = addToolBar("Control");
  QToolBar* tb_julia = addToolBar("Julia Control");
  m_fractalWidget->addControls(tb);
  m_fractalWidget->addJuliaControls(tb_julia);
  tb->addWidget(menuButton);
  
  m_fractalWidget->start();
  
  setCentralWidget(m_fractalWidget);
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
  m_fractalWidget->setColoursDialog();
}

void MainWindow::colours_end()
{
  m_fractalWidget->setColoursDialog(false);
}

void MainWindow::copy()
{
  QApplication::clipboard()->setImage(m_fractalWidget->pixmap().toImage());
}

void MainWindow::newFractalWidget()
{
  MainWindow* m = new MainWindow(m_fractalWidget);
  m->show();
}

void MainWindow::pause(bool checked)
{
  checked ? m_fractalWidget->pause(): m_fractalWidget->cont();
}

void MainWindow::refresh()
{
  m_fractalWidget->refresh();
}
