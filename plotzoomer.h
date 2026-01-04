////////////////////////////////////////////////////////////////////////////////
// Name:      plotzoomer.h
// Purpose:   Declaration of class PlotZoomer
// Author:    Anton van Wezenbeek
// Copyright: (c) 2017-2026 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#pragma once 

#include <QtGui>
#include <QStatusBar>
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
  PlotZoomer(QWidget* widget, QStatusBar* bar, bool doReplot = true);
  
protected:  
  virtual QSizeF minZoomSize() const override;
  virtual void rescale() override;
  virtual QwtText trackerTextF( const QPointF & ) const override;
  virtual void widgetKeyPressEvent(QKeyEvent *) override;
  virtual void widgetMouseDoubleClickEvent(QMouseEvent *) override;
  virtual void widgetMousePressEvent(QMouseEvent *) override;
  virtual void widgetMouseReleaseEvent(QMouseEvent *) override;

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
  QPointF m_Point;
  QStatusBar* m_statusBar;

  bool m_inZoom = false;
  bool m_alignCanvasToScales[ QwtPlot::axisCnt ];
};
