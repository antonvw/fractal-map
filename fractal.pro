################################################################################
# Name:      fractal.pro
# Purpose:   Qt project file
# Author:    Anton van Wezenbeek
# Copyright: (c) 2017 Anton van Wezenbeek
################################################################################

TEMPLATE = app
QT += widgets
RC_FILE = fractal.rc

equals( QMAKE_CXX, cl) {
  include ( c:\qwt\features\qwt.prf )
}
    
!equals( QMAKE_CXX, cl) {
  include ( /usr/local/qwt/features/qwt.prf )
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
