////////////////////////////////////////////////////////////////////////////////
// Name:      fractalwidget.cpp
// Purpose:   Implementation of class FractalWidget
// Author:    Anton van Wezenbeek
// Copyright: (c) 2013 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#include <QtGui>
#include "plotzoomer.h"
#include "fractalwidget.h"

PlotZoomer::PlotZoomer(QWidget* widget, bool doReplot)
  : QwtPlotZoomer(widget, doReplot)
{
  setKeyPattern( KeyRedo, Qt::Key_Up );
  setKeyPattern( KeyUndo, Qt::Key_Down );
  setKeyPattern( KeyHome, Qt::Key_Escape );
    
  setTrackerPen(QColor(Qt::white));
  setTrackerMode(AlwaysOn);
}

QSizeF PlotZoomer::minZoomSize() const
{
  // default uses 10e4
  return QSizeF(
    zoomStack()[0].width() / 10e6,
    zoomStack()[0].height() / 10e6);
}

QwtText PlotZoomer::trackerTextF( const QPointF &pos ) const
{
  QString text;

  switch (rubberBand())
  {
    case HLineRubberBand:
      text.sprintf("%.6f", pos.y());
      break;
    case VLineRubberBand:
      text.sprintf("%.6f", pos.x());
      break;
    default:
      text.sprintf("%.6f, %.6f", pos.x(), pos.y());
  }
  
  return QwtText(text);
}

void PlotZoomer::widgetMouseDoubleClickEvent(QMouseEvent*)
{
  FractalWidget* fw = (FractalWidget *)plot();
  
  fw->doubleClicked();
}
  