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

/// This class offers facility to zoom in on the plot.
class PlotZoomer: public QwtPlotZoomer
{
  Q_OBJECT

public:
  /// Constructor.
  PlotZoomer(QWidget* widget, bool doReplot = true);
  
protected:  
  virtual bool eventFilter( QObject *, QEvent * );
  virtual QSizeF minZoomSize() const;
  virtual void rescale();
  virtual QwtText trackerTextF( const QPointF & ) const;
  virtual void widgetKeyPressEvent(QKeyEvent *);
  virtual void widgetMouseDoubleClickEvent(QMouseEvent *);
  
private slots:
  void scrollBarValueChanged( Qt::Orientation, double, double );

private:
  void layoutScrollBars( const QRect & );
  bool needScrollBar( Qt::Orientation ) const;
  int oppositeAxis( int ) const;
  void updateScrollBars();

  QWidget* m_cornerWidget;
  ScrollBar* m_hScrollBar;
  ScrollBar* m_vScrollBar;

  bool m_inZoom;
  bool m_alignCanvasToScales[ QwtPlot::axisCnt ];
};
