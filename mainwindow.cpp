////////////////////////////////////////////////////////////////////////////////
// Name:      mainwindow.cpp
// Purpose:   Implementation of class MainWindow
// Author:    Anton van Wezenbeek
// Copyright: (c) 2017 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#include <QtGui>
#include <QMessageBox>
#include <QMenu>
#include <QPushButton>
#include <QSettings>
#include <qwt_global.h>
#include "mainwindow.h"
#include "fractalwidget.h"

void menuItem(QMenu* menu, 
  const char* label, 
  const QObject* object, 
  const char* method, 
  const QKeySequence& sequence = QKeySequence(),
  bool separator = false,
  bool checkable = false)
{
  auto* action = menu->addAction(label);
  object->connect(action, 
    !checkable ? SIGNAL(triggered()): SIGNAL(toggled(bool)), object, method);

  if (!sequence.isEmpty())
  {
    action->setShortcut(sequence);
  }

  if (checkable)
  {
    action->setCheckable(true);
  }

  if (separator)
  {
    menu->addSeparator();
  }
}

MainWindow::MainWindow(QWidget* parent, FractalWidget* fw)
  : QMainWindow(parent)
{
  if (fw != nullptr)
  {
    m_fractalWidget = new FractalWidget(*fw, statusBar());
    
    if (parent != nullptr)
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
      settings.value("depth", 64).toInt(),
      settings.value("julia real", 0.9).toDouble(),
      settings.value("julia imag", 1.1).toDouble(),
      settings.value("julia exponent", 2).toDouble(),
      settings.value("axes", false).toBool());
      
    resize(QSize(300, 300)); // initial size
      
    restoreGeometry(settings.value("mainWindowGeometry").toByteArray());
  }
    
  qRegisterMetaType<QImage>("QImage");
  
  auto* menuButton = new QPushButton();
  menuButton->setFixedWidth(20);

  auto* menu = new QMenu(this);

  menuItem(menu, "About", this, SLOT(about()), QKeySequence(), true);
  menuItem(menu, "Colours From Begin...", &m_fractalWidget->fractalControl(), SLOT(setColoursDialogBegin()));
  menuItem(menu, "Colours From End...", &m_fractalWidget->fractalControl(), SLOT(setColoursDialogEnd()), QKeySequence(), true);
  menuItem(menu, "Images...", &m_fractalWidget->fractalControl(), SLOT(setImages()), QKeySequence(), true);
  menuItem(menu, "Copy", m_fractalWidget, SLOT(copy()), QKeySequence::Copy);
  menuItem(menu, "Refresh", m_fractalWidget->renderer(), SLOT(refresh()), QKeySequence::Refresh);
  menuItem(menu, "Restart", m_fractalWidget->renderer(), SLOT(restart()), QKeySequence("Ctrl+R"), true);
  menuItem(menu, "Zoom In", m_fractalWidget, SLOT(zoomIn()), QKeySequence("Ctrl+Z"));
  menuItem(menu, "Zoom Out", m_fractalWidget, SLOT(zoomOut()), QKeySequence("Ctrl+X"));
  menuItem(menu, "Auto Zoom", m_fractalWidget, SLOT(autoZoom()), QKeySequence("Ctrl+A"));
  menuItem(menu, "Stop", m_fractalWidget, SLOT(autoZoomStop()), QKeySequence("Ctrl+S"), true);
  menuItem(menu, "New", this, SLOT(newFractalWidget()), QKeySequence::New, true);
  menuItem(menu, "Pause", m_fractalWidget->renderer(), SLOT(pause(bool)), QKeySequence(), false, true);

  menuButton->setMenu(menu);
  
  connect(qApp, SIGNAL(lastWindowClosed()), m_fractalWidget, SLOT(save()));

  auto* tb = addToolBar("Control");
  tb->setOrientation(Qt::Vertical); // TODO: does not work
  tb->setObjectName("control");
  auto* tb_julia = new QToolBar("Julia Control");
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

void MainWindow::closeEvent(QCloseEvent* /* event */) 
{
  QSettings settings;
  settings.setValue("mainWindowGeometry", saveGeometry());
  settings.setValue("mainWindowState", saveState());
}
    
void MainWindow::newFractalWidget()
{
  auto* m = new MainWindow(this, m_fractalWidget);
  m->show();
}
