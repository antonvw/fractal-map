////////////////////////////////////////////////////////////////////////////////
// Name:      mainwindow.cpp
// Purpose:   Implementation of class MainWindow
// Author:    Anton van Wezenbeek
// Copyright: (c) 2012 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

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
      settings.value("center", QPointF(0,0)).toPointF(),
      settings.value("first pass", 1).toInt(),
      settings.value("last pass", 10).toInt(),
      settings.value("julia real", 0.9).toDouble(),
      settings.value("julia imag", 1.1).toDouble(),
      settings.value("julia exponent", 2).toDouble());
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
  QAction* skip = menu->addAction("Skip");
  skip->setShortcut(QKeySequence("Ctrl+S"));
  menu->addSeparator();
  QAction* newAction = menu->addAction("New");
  newAction->setShortcut(QKeySequence::New);
  menu->addSeparator();
  QAction* pause = new QAction("Pause", menu);
  pause->setCheckable(true);
  menu->addAction(pause);
  menuButton->setMenu(menu);
  
  connect(about, SIGNAL(triggered()), this, SLOT(about()));    
  connect(colours_begin, SIGNAL(triggered()), m_fractalWidget, SLOT(setColoursDialogBegin()));
  connect(colours_end, SIGNAL(triggered()), m_fractalWidget, SLOT(setColoursDialogEnd()));
  connect(copy, SIGNAL(triggered()), m_fractalWidget, SLOT(copy()));
  connect(menu, SIGNAL(clicked()), this, SLOT(menu()));
  connect(newAction, SIGNAL(triggered()), this, SLOT(newFractalWidget()));
  connect(pause, SIGNAL(toggled(bool)), m_fractalWidget, SLOT(pause(bool)));
  connect(qApp, SIGNAL(lastWindowClosed()), m_fractalWidget, SLOT(save()));
  connect(refresh, SIGNAL(triggered()), m_fractalWidget, SLOT(refresh()));
  connect(skip, SIGNAL(triggered()), m_fractalWidget, SLOT(skip()));

  QToolBar* tb = addToolBar("Control");
  QToolBar* tb_julia = new QToolBar("Julia Control");
  addToolBar(Qt::BottomToolBarArea, tb_julia);
  m_fractalWidget->addControls(tb);
  m_fractalWidget->addJuliaControls(tb_julia);
  tb->addWidget(menuButton);
  
  setCentralWidget(m_fractalWidget);
  setWindowTitle("Fractal Map");
  
  resize(550, 400);
  
  m_fractalWidget->start();
}

void MainWindow::about()
{
  QMessageBox::about(this, 
    "About " + windowTitle(),
    QString("This application shows a fractal map.\nBuilt using Qt %1").
      arg(QT_VERSION_STR));
}

void MainWindow::newFractalWidget()
{
  MainWindow* m = new MainWindow(m_fractalWidget);
  m->show();
}
