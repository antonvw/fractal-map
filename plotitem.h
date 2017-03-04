////////////////////////////////////////////////////////////////////////////////
// Name:      plotitem.cpp
// Purpose:   Implementation of class FractalPlotItem
// Author:    Anton van Wezenbeek
// Copyright: (c) 2017 Anton van Wezenbeek
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
    const QRectF &rect) const override;
    
  virtual int rtti() const override;
};
