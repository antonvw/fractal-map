////////////////////////////////////////////////////////////////////////////////
// Name:      main.cpp
// Purpose:   main for fractal-map application
// Author:    Anton van Wezenbeek
// Copyright: (c) 2012 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  
  QCoreApplication::setOrganizationName("Coffee Tigers");
  QCoreApplication::setApplicationName("fractal-map");
  
  MainWindow win;
  win.show();
  
  return app.exec();
}
