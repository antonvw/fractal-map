////////////////////////////////////////////////////////////////////////////////
// Name:      mainwindow.h
// Purpose:   Declaration of class MainWindow
// Author:    Anton van Wezenbeek
// Copyright: (c) 2015 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QMainWindow>

class FractalWidget;

// This class offers the main window showing
// a fractal widget.
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  // Constructor.
  MainWindow(QWidget* parent = NULL, FractalWidget* fractalwidget = NULL);
private slots:
  void about();
  void newFractalWidget();
private:
  void closeEvent(QCloseEvent *event); 
  FractalWidget* m_fractalWidget;
};
