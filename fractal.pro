################################################################################
# Name:      fractal.pro
# Purpose:   Qt project file
# Author:    Anton van Wezenbeek
# Copyright: (c) 2015 Anton van Wezenbeek
################################################################################

TEMPLATE = app
QT += widgets
RC_FILE = fractal.rc

equals( QMAKE_CXX, cl) {
  include ( c:\qwt-6.1.2\features\qwt.prf )
}
    
!equals( QMAKE_CXX, cl) {
  include ( /usr/local/qwt-6.1.2/features/qwt.prf )
}

HEADERS += \
  fractal.h \
  fractalgeometry.h \
  fractalrenderer.h \
  fractalwidget.h \
  mainwindow.h \
  plotitem.h \
  plotzoomer.h \
  scrollbar.h 
  
SOURCES += \
  fractal.cpp \
  fractalgeometry.cpp \
  fractalrenderer.cpp \
  fractalwidget.cpp \
  main.cpp \
  mainwindow.cpp \
  plotitem.cpp \
  plotzoomer.cpp \
  scrollbar.cpp
