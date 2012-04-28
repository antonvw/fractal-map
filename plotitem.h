////////////////////////////////////////////////////////////////////////////////
// Name:      fractalwidget.cpp
// Purpose:   Implementation of class FractalWidget
// Author:    Anton van Wezenbeek
// Copyright: (c) 2012 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#pragma once
 
#include <QtGui>
#include <qwt_plot_item.h>

class FractalPlotItem: public QwtPlotItem
{
public:
  FractalPlotItem();

  virtual void draw(QPainter *p,
    const QwtScaleMap&, 
    const QwtScaleMap&,
    const QRectF &rect) const;
    
  virtual int rtti() const;
};
