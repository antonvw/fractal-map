################################################################################
# Name:      fractal.pro
# Purpose:   Qt project file
# Author:    Anton van Wezenbeek
# Copyright: (c) 2014 Anton van Wezenbeek
################################################################################

TEMPLATE = app
CONFIG += qwt
RC_FILE = fractal.rc

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
