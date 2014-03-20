////////////////////////////////////////////////////////////////////////////////
// Name:      mainwindow.cpp
// Purpose:   Implementation of class MainWindow
// Author:    Anton van Wezenbeek
// Copyright: (c) 2014 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#include <QtGui>
#include <QMessageBox>
#include <QMenu>
#include <QPushButton>
#include <QSettings>
#include <qwt_global.h>
#include "mainwindow.h"
#include "fractalwidget.h"

MainWindow::MainWindow(QWidget* parent, FractalWidget* fw)
  : QMainWindow(parent)
{
  if (fw != NULL)
  {
    m_fractalWidget = new FractalWidget(*fw, statusBar());
    
    if (parent != NULL)
    {
      resize(parent->size());
    }
  }
  else
  {
    QSettings settings;
    
    m_fractalWidget = new FractalWidget(
      this,
      statusBar(),
      settings.value("fractal", "julia set 4").toString(),
      settings.value("colours", 128).toInt(),
      settings.value("dir", "").toString(),
      settings.value("diverge", 2).toDouble(),
      QwtInterval(-2, 2),
      QwtInterval(-2, 2),
      settings.value("first pass", 1).toInt(),
      settings.value("last pass", 10).toInt(),
      settings.value("julia real", 0.9).toDouble(),
      settings.value("julia imag", 1.1).toDouble(),
      settings.value("julia exponent", 2).toDouble(),
      settings.value("axes", false).toBool());
      
    resize(QSize(300, 300)); // initial size
      
    restoreGeometry(settings.value("mainWindowGeometry").toByteArray());
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
  QAction* images = menu->addAction("Images...");
  menu->addSeparator();
  QAction* copy = menu->addAction("Copy");
  copy->setShortcuts(QKeySequence::Copy);
  QAction* refresh = menu->addAction("Refresh");
  refresh->setShortcuts(QKeySequence::Refresh);
  QAction* skip = menu->addAction("Skip");
  skip->setShortcut(QKeySequence("Ctrl+S"));
  menu->addSeparator();
  QAction* zoom = menu->addAction("Zoom");
  QAction* autoZoom = menu->addAction("Auto Zoom");
  zoom->setShortcut(QKeySequence("Ctrl+Z"));
  menu->addSeparator();
  QAction* newAction = menu->addAction("New");
  newAction->setShortcut(QKeySequence::New);
  menu->addSeparator();
  QAction* pause = new QAction("Pause", menu);
  pause->setCheckable(true);
  menu->addAction(pause);
  menuButton->setMenu(menu);
  
  connect(about, SIGNAL(triggered()), this, SLOT(about()));    
  connect(colours_begin, SIGNAL(triggered()), &m_fractalWidget->geometry(), SLOT(setColoursDialogBegin()));
  connect(colours_end, SIGNAL(triggered()), &m_fractalWidget->geometry(), SLOT(setColoursDialogEnd()));
  connect(images, SIGNAL(triggered()), &m_fractalWidget->geometry(), SLOT(setImages()));
  connect(copy, SIGNAL(triggered()), m_fractalWidget, SLOT(copy()));
  connect(newAction, SIGNAL(triggered()), this, SLOT(newFractalWidget()));
  connect(pause, SIGNAL(toggled(bool)), m_fractalWidget->renderer(), SLOT(pause(bool)));
  connect(qApp, SIGNAL(lastWindowClosed()), m_fractalWidget, SLOT(save()));
  connect(refresh, SIGNAL(triggered()), m_fractalWidget->renderer(), SLOT(refresh()));
  connect(skip, SIGNAL(triggered()), m_fractalWidget->renderer(), SLOT(skip()));
  connect(zoom, SIGNAL(triggered()), m_fractalWidget, SLOT(zoom()));
  connect(autoZoom, SIGNAL(triggered()), m_fractalWidget, SLOT(autoZoom()));

  QToolBar* tb = addToolBar("Control");
  tb->setObjectName("control");
  QToolBar* tb_julia = new QToolBar("Julia Control");
  tb_julia->setObjectName("julia");
  addToolBar(Qt::BottomToolBarArea, tb_julia);
  tb->addWidget(menuButton);
  m_fractalWidget->addControls(tb);
  addToolBarBreak();
  m_fractalWidget->addGeometryControls(tb);
  m_fractalWidget->addJuliaControls(tb_julia);
  
  restoreState(QSettings().value("mainWindowState").toByteArray());
  
  setCentralWidget(m_fractalWidget);
  setWindowTitle("Fractal Map");
  
  m_fractalWidget->renderer()->start();
}

void MainWindow::about()
{
  QMessageBox::about(this, 
    "About " + windowTitle(),
    QString("This application shows a fractal map.\nBuilt using Qt %1 and Qwt %2")
      .arg(QT_VERSION_STR)
      .arg(QWT_VERSION_STR));
}

void MainWindow::closeEvent(QCloseEvent *event) 
{
  QSettings settings;
  settings.setValue("mainWindowGeometry", saveGeometry());
  settings.setValue("mainWindowState", saveState());
}
    
void MainWindow::newFractalWidget()
{
  MainWindow* m = new MainWindow(this, m_fractalWidget);
  m->show();
}
