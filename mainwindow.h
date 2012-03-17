#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// This class offers the main window.
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  // Constructor.
  MainWindow(QWidget *parent = 0);
private slots:
  void about();
  void newFractal();
private:
  QWidget* newFractalWidget(QWidget* parent);
};
#endif
