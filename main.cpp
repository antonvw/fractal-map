#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  
  QCoreApplication::setOrganizationName("Coffee Company");
  QCoreApplication::setApplicationName("fractal-map");
  
  MainWindow win;
  win.show();
  
  return app.exec();
}
