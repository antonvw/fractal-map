////////////////////////////////////////////////////////////////////////////////
// Name:      plotzoomer.h
// Purpose:   Declaration of class PlotZoomer
// Author:    Anton van Wezenbeek
// Copyright: (c) 2014 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#pragma once 

#include <QtGui>
#include <qglobal.h>
#include <qwt_plot.h>
#include <qwt_plot_zoomer.h>

class ScrollBar;

class PlotZoomer: public QwtPlotZoomer
{
  Q_OBJECT

public:
  /// Constructor.
  PlotZoomer(QWidget* widget, bool doReplot = true);
  
protected:  
  virtual QSizeF minZoomSize() const;
  virtual QwtText trackerTextF( const QPointF & ) const;
  virtual void widgetMouseDoubleClickEvent(QMouseEvent *);
  
private:  
  virtual bool eventFilter( QObject *, QEvent * );
  virtual void layoutScrollBars( const QRect & );
  virtual void rescale();
  virtual void updateScrollBars();

private slots:
  void scrollBarValueChanged( Qt::Orientation, double, double );

private:
  bool needScrollBar( Qt::Orientation ) const;
  int oppositeAxis( int ) const;

  QWidget* m_cornerWidget;
  ScrollBar* m_hScrollBar;
  ScrollBar* m_vScrollBar;

  bool m_inZoom;
  bool m_alignCanvasToScales[ QwtPlot::axisCnt ];
};
