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

class ScrollData;
class ScrollBar;

class PlotZoomer: public QwtPlotZoomer
{
  Q_OBJECT

public:
  PlotZoomer(QWidget* widget, bool doReplot = true);
  
  enum ScrollBarPosition
  {
    AttachedToScale,
    OppositeToScale
  };

protected:  
  virtual QSizeF minZoomSize() const;
  virtual QwtText trackerTextF( const QPointF & ) const;
  virtual void widgetMouseDoubleClickEvent(QMouseEvent *);
  
private:  
  ScrollBar *horizontalScrollBar() const;
  ScrollBar *verticalScrollBar() const;

  void setHScrollBarMode( Qt::ScrollBarPolicy );
  void setVScrollBarMode( Qt::ScrollBarPolicy );

  QWidget* cornerWidget() const;

  virtual bool eventFilter( QObject *, QEvent * );
  virtual void layoutScrollBars( const QRect & );
  virtual void rescale();
  virtual ScrollBar *scrollBar( Qt::Orientation );
  virtual void updateScrollBars();

private slots:
  void scrollBarMoved( Qt::Orientation, double, double );

private:
  bool needScrollBar( Qt::Orientation ) const;
  int oppositeAxis( int ) const;

  QWidget* m_cornerWidget;

  ScrollData* m_hScrollData;
  ScrollData* m_vScrollData;

  bool m_inZoom;
  bool m_alignCanvasToScales[ QwtPlot::axisCnt ];
};
