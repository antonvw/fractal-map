////////////////////////////////////////////////////////////////////////////////
// Name:      plotzoomer.cpp
// Purpose:   Implementation of class PlotZoomer
// Author:    Anton van Wezenbeek
// Copyright: (c) 2012 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#pragma once 

#include <QtGui>
#include <qwt_plot_zoomer.h>

class PlotZoomer: public QwtPlotZoomer
{
public:
  PlotZoomer(QwtPlotCanvas* canvas);
protected:  
  virtual QSizeF minZoomSize() const;
  virtual QwtText trackerTextF( const QPointF & ) const;
  virtual void widgetMouseDoubleClickEvent(QMouseEvent *);
};
