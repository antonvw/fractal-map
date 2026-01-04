################################################################################
# Name:      fractal.pro
# Purpose:   Qt project file
# Author:    Anton van Wezenbeek
# Copyright: (c) 2017-2026 Anton van Wezenbeek
################################################################################

TEMPLATE = app
QT += widgets
RC_FILE = fractal.rc

win32 {
  include ( c:\qwt\features\qwt.prf )
}

linux-g++ {
  include ( /usr/local/qwt/features/qwt.prf )
}

macx {
  include ( /usr/local/Cellar/homebrew/Cellar/qwt/6.3.0/features/qwt.prf )
}

HEADERS += \
  fractal.h \
  fractalcontrol.h \
  fractalgeometry.h \
  fractalrenderer.h \
  fractalwidget.h \
  mainwindow.h \
  plotitem.h \
  plotzoomer.h \
  scrollbar.h

SOURCES += \
  fractal.cpp \
  fractalcontrol.cpp \
  fractalgeometry.cpp \
  fractalrenderer.cpp \
  fractalwidget.cpp \
  main.cpp \
  mainwindow.cpp \
  plotitem.cpp \
  plotzoomer.cpp \
  scrollbar.cpp
