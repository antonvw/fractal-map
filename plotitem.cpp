////////////////////////////////////////////////////////////////////////////////
// Name:      plotitem.cpp
// Purpose:   Implementation of class FractalPlotItem
// Author:    Anton van Wezenbeek
// Copyright: (c) 2012 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#include <QtGui>
#include <qwt_painter.h>
#include "plotitem.h"
#include "fractalwidget.h"

FractalPlotItem::FractalPlotItem()
{
  setItemAttribute(AutoScale);
  setRenderHint(QwtPlotItem::RenderAntialiased, true);
  setZ(1);
}

void FractalPlotItem::draw(QPainter *p, 
  const QwtScaleMap&,
  const QwtScaleMap&,
  const QRectF& r) const
{
  const FractalWidget* fw = (FractalWidget *)plot();
  
  QwtPainter::drawPixmap(p, r, fw->fractalPixmap());
}

int FractalPlotItem::rtti() const
{
  return QwtPlotItem::Rtti_PlotUserItem;
}
