////////////////////////////////////////////////////////////////////////////////
// Name:      mainwindow.h
// Purpose:   Declaration of class MainWindow
// Author:    Anton van Wezenbeek
// Copyright: (c) 2017 Anton van Wezenbeek
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
  // Default constructor.
  MainWindow(
    QWidget* parent = nullptr, FractalWidget* fractalwidget = nullptr);
private slots:
  void about();
  void newFractalWidget();
private:
  virtual void closeEvent(QCloseEvent *event) override; 
  FractalWidget* m_fractalWidget;
};
