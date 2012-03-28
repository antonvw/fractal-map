////////////////////////////////////////////////////////////////////////////////
// Name:      mainwindow.h
// Purpose:   Declaration of class MainWindow
// Author:    Anton van Wezenbeek
// Copyright: (c) 2012 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#ifndef _MAINWINDOW_H
#define _MAINWINDOW_H

#include <QMainWindow>

class FractalWidget;

// This class offers the main window showing
// a fractal widget.
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  // Constructor.
  MainWindow(FractalWidget* fractal = NULL, QWidget *parent = 0);
private slots:
  void about();
  void newFractalWidget();
private:
  FractalWidget* m_fractalWidget;
};
#endif
