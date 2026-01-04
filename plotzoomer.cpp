////////////////////////////////////////////////////////////////////////////////
// Name:      plotzoomer.cpp
// Purpose:   Implementation of class PlotZoomer
// Author:    Anton van Wezenbeek
// Copyright: (c) 2017-2026 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#include <QtGui>
#include <QEvent>
#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>
#include <qwt_scale_engine.h>
#include <qwt_scale_widget.h>

#include "plotzoomer.h"
#include "fractalwidget.h"
#include "scrollbar.h"

PlotZoomer::PlotZoomer(QWidget* widget, QStatusBar* bar, bool doReplot)
  : QwtPlotZoomer(widget, doReplot)
  , m_cornerWidget( new QWidget( canvas() ))
  , m_hScrollBar( new ScrollBar( ScrollBar::AttachedToScale, Qt::Horizontal, canvas() ))
  , m_vScrollBar( new ScrollBar( ScrollBar::OppositeToScale, Qt::Vertical, canvas() ))
  , m_statusBar(bar)
{
  m_cornerWidget->setAutoFillBackground( true );
  m_cornerWidget->setPalette( plot()->palette() );
  
  m_hScrollBar->setPalette( plot()->palette() );
  m_vScrollBar->setPalette( plot()->palette() );
    
  setKeyPattern( KeyRedo, Qt::Key_F11 );
  setKeyPattern( KeyUndo, Qt::Key_F12 );
    
  setTrackerPen(QColor(Qt::white));
  setTrackerMode(ActiveOnly);
  
  for ( int axis = 0; axis < QwtPlot::axisCnt; axis++ )
    m_alignCanvasToScales[ axis ] = false;
    
  connect( m_hScrollBar,
    SIGNAL( valueChanged( Qt::Orientation, double, double ) ),
    SLOT( scrollBarValueChanged( Qt::Orientation, double, double ) ) );
    
  connect( m_vScrollBar,
    SIGNAL( valueChanged( Qt::Orientation, double, double ) ),
    SLOT( scrollBarValueChanged( Qt::Orientation, double, double ) ) );
    
  updateScrollBars();
}

void PlotZoomer::layoutScrollBars( const QRect &rect )
{
  int hPos = xAxis();
  if ( m_hScrollBar->position() == ScrollBar::OppositeToScale )
    hPos = oppositeAxis( hPos );

  int vPos = yAxis();
  if ( m_vScrollBar->position() == ScrollBar::OppositeToScale )
    vPos = oppositeAxis( vPos );

  const int hdim = m_hScrollBar->extent();
  const int vdim = m_vScrollBar->extent();

  if ( m_hScrollBar->isVisible() )
  {
    int x = rect.x();
    int y = ( hPos == QwtPlot::xTop )
      ? rect.top() : rect.bottom() - hdim + 1;
    int w = rect.width();

    if ( m_vScrollBar->isVisible() )
    {
      if ( vPos == QwtPlot::yLeft )
        x += vdim;
      w -= vdim;
    }

    m_hScrollBar->setGeometry( x, y, w, hdim );
  }

  if ( m_vScrollBar->isVisible() )
  {
    int pos = yAxis();
    if ( m_vScrollBar->position() == ScrollBar::OppositeToScale )
      pos = oppositeAxis( pos );

    int x = ( vPos == QwtPlot::yLeft )
      ? rect.left() : rect.right() - vdim + 1;
    int y = rect.y();

    int h = rect.height();

    if ( m_hScrollBar->isVisible() )
    {
      if ( hPos == QwtPlot::xTop )
        y += hdim;

      h -= hdim;
    }

    m_vScrollBar->setGeometry( x, y, vdim, h );
  }

  if ( m_hScrollBar->isVisible() && m_vScrollBar->isVisible() )
  {
    QRect cornerRect(
      m_vScrollBar->pos().x(), m_hScrollBar->pos().y(),
      vdim, hdim );
      
    m_cornerWidget->setGeometry( cornerRect );
  }
}

QSizeF PlotZoomer::minZoomSize() const
{
  // default uses 10e4
  return QSizeF(
    zoomStack()[0].width() / 10e6,
    zoomStack()[0].height() / 10e6);
}

bool PlotZoomer::needScrollBar( Qt::Orientation orientation ) const
{
  Qt::ScrollBarPolicy mode;
  double zoomMin, zoomMax, baseMin, baseMax;

  if ( orientation == Qt::Horizontal )
  {
    mode = m_hScrollBar->mode();
    baseMin = zoomBase().left();
    baseMax = zoomBase().right();
    zoomMin = zoomRect().left();
    zoomMax = zoomRect().right();
  }
  else
  {
    mode = m_vScrollBar->mode();
    baseMin = zoomBase().top();
    baseMax = zoomBase().bottom();
    zoomMin = zoomRect().top();
    zoomMax = zoomRect().bottom();
  }

  bool needed = false;
  switch( mode )
  {
    case Qt::ScrollBarAlwaysOn:
      needed = true;
      break;
    case Qt::ScrollBarAlwaysOff:
      needed = false;
      break;
    default:
    {
      if ( baseMin < zoomMin || baseMax > zoomMax )
        needed = true;
      break;
    }
  }
  return needed;
}

int PlotZoomer::oppositeAxis( int axis ) const
{
  switch( axis )
  {
    case QwtPlot::xBottom:
      return QwtPlot::xTop;
    case QwtPlot::xTop:
      return QwtPlot::xBottom;
    case QwtPlot::yLeft:
      return QwtPlot::yRight;
    case QwtPlot::yRight:
      return QwtPlot::yLeft;
    default:
      break;
  }

  return axis;
}

void PlotZoomer::rescale()
{
  QwtScaleWidget *xScale = plot()->axisWidget( xAxis() );
  QwtScaleWidget *yScale = plot()->axisWidget( yAxis() );

  if (zoomRectIndex() <= 0)
  {
    if (m_inZoom)
    {
      xScale->setMinBorderDist( 0, 0 );
      yScale->setMinBorderDist( 0, 0 );

      QwtPlotLayout *layout = plot()->plotLayout();

      for ( int axis = 0; axis < QwtPlot::axisCnt; axis++ )
        layout->setAlignCanvasToScale( axis, m_alignCanvasToScales );

      m_inZoom = false;
    }
  }
  else
  {
    if (!m_inZoom)
    {
      /*
       We set a minimum border distance.
       Otherwise the canvas size changes when scrolling,
       between situations where the major ticks are at
       the canvas borders (requiring extra space for the label)
       and situations where all labels can be painted below/top
       or left/right of the canvas.
       */
      int start, end;

      xScale->getBorderDistHint( start, end );
      xScale->setMinBorderDist( start, end );

      yScale->getBorderDistHint( start, end );
      yScale->setMinBorderDist( start, end );

      QwtPlotLayout *layout = plot()->plotLayout();
      for ( int axis = 0; axis < QwtPlot::axisCnt; axis++ )
      {
        m_alignCanvasToScales[axis] = 
          layout->alignCanvasToScale( axis );
      }

      layout->setAlignCanvasToScales( false );

      m_inZoom = true;
    }
  }

  QwtPlotZoomer::rescale();
  
  updateScrollBars();
}

void PlotZoomer::scrollBarValueChanged(
  Qt::Orientation o, double min, double max )
{
  Q_UNUSED( max );

  if ( o == Qt::Horizontal )
    moveTo( QPointF( min, zoomRect().top() ) );
  else
    moveTo( QPointF( zoomRect().left(), min ) );
      
  emit zoomed( zoomRect() );
}

QwtText PlotZoomer::trackerTextF( const QPointF &pos ) const
{
  QString text;

  switch (rubberBand())
  {
    case HLineRubberBand:
      text.asprintf("%.6f", pos.y());
      break;
    case VLineRubberBand:
      text.asprintf("%.6f", pos.x());
      break;
    default:
      text.asprintf("%.6f, %.6f", pos.x(), pos.y());
  }
  
  return QwtText(text);
}

void PlotZoomer::updateScrollBars()
{
  int xAx = xAxis();
  int yAx = yAxis();

  if ( m_hScrollBar->position() == ScrollBar::OppositeToScale )
    xAx = oppositeAxis( xAx );

  if ( m_vScrollBar->position() == ScrollBar::OppositeToScale )
    yAx = oppositeAxis( yAx );

  QwtPlotLayout *layout = plot()->plotLayout();

  bool showHScrollBar = needScrollBar( Qt::Horizontal );
  
  if ( showHScrollBar )
  {
    m_hScrollBar->setBase( zoomBase().left(), zoomBase().right() );
    m_hScrollBar->moveSlider( zoomRect().left(), zoomRect().right() );

    if ( !m_hScrollBar->isVisibleTo( canvas() ) )
    {
      m_hScrollBar->show();
      layout->setCanvasMargin( layout->canvasMargin( xAx )
        + m_hScrollBar->extent(), xAx );
    }
  }
  else
  {
    m_hScrollBar->hide();
    layout->setCanvasMargin( layout->canvasMargin( xAx )
      - m_hScrollBar->extent(), xAx );
  }

  bool showVScrollBar = needScrollBar( Qt::Vertical );
  
  if ( showVScrollBar )
  {
    m_vScrollBar->setBase( zoomBase().top(), zoomBase().bottom() );
    m_vScrollBar->moveSlider( zoomRect().top(), zoomRect().bottom() );

    if ( !m_vScrollBar->isVisibleTo( canvas() ) )
    {
      m_vScrollBar->show();
      layout->setCanvasMargin( layout->canvasMargin( yAx )
        + m_vScrollBar->extent(), yAx );
    }
  }
  else
  {
    m_vScrollBar->hide();
    layout->setCanvasMargin( layout->canvasMargin( yAx )
      - m_vScrollBar->extent(), yAx );
  }

  if ( showHScrollBar && showVScrollBar )
  {
    m_cornerWidget->show();
  }
  else
  {
    m_cornerWidget->hide();
  }

  layoutScrollBars( canvas()->contentsRect() );
  plot()->updateLayout();
}

void PlotZoomer::widgetKeyPressEvent(QKeyEvent* event)
{
  if (
    event->key() == Qt::Key_Left || 
    event->key() == Qt::Key_Right ||
    event->key() == Qt::Key_Home ||
    event->key() == Qt::Key_End)
  {
    m_hScrollBar->event(event);
  }
  else if (
    event->key() == Qt::Key_Up || 
    event->key() == Qt::Key_Down ||
    event->key() == Qt::Key_PageUp || 
    event->key() == Qt::Key_PageDown)
  {
    m_vScrollBar->event(event);
  }
  else if (event->key() == Qt::Key_Plus)
  {
    if (zoomRectIndex() < (uint)zoomStack().size() - 1)
    {
      zoom(1);
    }
    
    m_statusBar->showMessage(QString("zoom stack: %1 (%2)")
      .arg(zoomRectIndex())
      .arg(zoomStack().size()));
  }
  else if (event->key() == Qt::Key_Minus)
  {
    if (zoomRectIndex() > 0)
    {
      zoom(-1);
    }
  
    m_statusBar->showMessage(QString("zoom stack: %1 (%2)")
      .arg(zoomRectIndex())
      .arg(zoomStack().size()));
  }
  else
  {
    QwtPlotZoomer::widgetKeyPressEvent(event);
  }
}

void PlotZoomer::widgetMouseDoubleClickEvent(QMouseEvent*)
{
  FractalWidget* fw = (FractalWidget *)plot();
  
  fw->doubleClicked();
  
  reset();
}

void PlotZoomer::widgetMousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton)
  {
    m_statusBar->showMessage(QString("zoom stack: %1 (%2)")
      .arg(zoomRectIndex())
      .arg(zoomStack().size()));
    
    QwtPlotZoomer::widgetMousePressEvent(event);
  }
  else if (event->button() == Qt::RightButton)
  {
    m_Point = event->screenPos();
  }
}

void PlotZoomer::widgetMouseReleaseEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton)
  {
    QwtPlotZoomer::widgetMouseReleaseEvent(event);
  }
  else if (event->button() == Qt::RightButton)
  {
    const float x = 100 * (event->screenPos().x() - m_Point.x());
    const float y = 100 * (event->screenPos().y() - m_Point.y());
    
    m_hScrollBar->setValue(m_hScrollBar->value() - x); 
    m_vScrollBar->setValue(m_vScrollBar->value() - y);
  }
}
