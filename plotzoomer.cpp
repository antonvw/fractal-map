////////////////////////////////////////////////////////////////////////////////
// Name:      plotzoomer.cpp
// Purpose:   Implementation of class PlotZoomer
// Author:    Anton van Wezenbeek
// Copyright: (c) 2014 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#include <qevent.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>
#include <qwt_scale_engine.h>
#include <qwt_scale_widget.h>
#include "scrollbar.h"


#include <QtGui>
#include "plotzoomer.h"
#include "fractalwidget.h"

class ScrollData
{
public:
  ScrollData(PlotZoomer::ScrollBarPosition pos)
  : m_scrollBar( NULL )
  , m_position( pos )
  , m_mode( Qt::ScrollBarAsNeeded )
  {
  }

  ~ScrollData()
  {
    delete m_scrollBar;
  }

  ScrollBar* m_scrollBar;
  const PlotZoomer::ScrollBarPosition m_position;
  Qt::ScrollBarPolicy m_mode;
};

PlotZoomer::PlotZoomer(QWidget* widget, bool doReplot)
  : QwtPlotZoomer(widget, doReplot)
  , m_cornerWidget( NULL )
  , m_hScrollData ( new ScrollData( PlotZoomer::AttachedToScale ))
  , m_vScrollData ( new ScrollData( PlotZoomer::OppositeToScale ))
  , m_inZoom( false )
{
  setKeyPattern( KeyRedo, Qt::Key_Up );
  setKeyPattern( KeyUndo, Qt::Key_Down );
  setKeyPattern( KeyHome, Qt::Key_Escape );
    
  setTrackerPen(QColor(Qt::white));
  setTrackerMode(AlwaysOn);
  
  for ( int axis = 0; axis < QwtPlot::axisCnt; axis++ )
    m_alignCanvasToScales[ axis ] = false;
}

bool PlotZoomer::eventFilter( QObject *object, QEvent *event )
{
    if ( object == canvas() )
    {
        switch( event->type() )
        {
            case QEvent::Resize:
            {
                int left, top, right, bottom;
                canvas()->getContentsMargins( &left, &top, &right, &bottom );

                QRect rect;
                rect.setSize( static_cast<QResizeEvent *>( event )->size() );
                rect.adjust( left, top, -right, -bottom );

                layoutScrollBars( rect );
                break;
            }
            case QEvent::ChildRemoved:
            {
                const QObject *child =
                    static_cast<QChildEvent *>( event )->child();

                if ( child == m_cornerWidget )
                    m_cornerWidget = NULL;
                else if ( child == m_hScrollData->m_scrollBar )
                    m_hScrollData->m_scrollBar = NULL;
                else if ( child == m_vScrollData->m_scrollBar )
                    m_vScrollData->m_scrollBar = NULL;
                break;
            }
            default:
                break;
        }
    }
    return QwtPlotZoomer::eventFilter( object, event );
}

QSizeF PlotZoomer::minZoomSize() const
{
  // default uses 10e4
  return QSizeF(
    zoomStack()[0].width() / 10e6,
    zoomStack()[0].height() / 10e6);
}

void PlotZoomer::rescale()
{
  QwtScaleWidget *xScale = plot()->axisWidget( xAxis() );
  QwtScaleWidget *yScale = plot()->axisWidget( yAxis() );

  if ( zoomRectIndex() <= 0 )
  {
      if ( m_inZoom )
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
      if ( !m_inZoom )
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

ScrollBar *PlotZoomer::scrollBar( Qt::Orientation orientation )
{
    ScrollBar *&sb = ( orientation == Qt::Vertical )
        ? m_vScrollData->m_scrollBar : m_hScrollData->m_scrollBar;

    if ( sb == NULL )
    {
        sb = new ScrollBar( orientation, canvas() );
        sb->hide();
        connect( sb,
            SIGNAL( valueChanged( Qt::Orientation, double, double ) ),
            this, SLOT( scrollBarMoved( Qt::Orientation, double, double ) ) );
    }
    return sb;
}

ScrollBar *PlotZoomer::horizontalScrollBar() const
{
  return m_hScrollData->m_scrollBar;
}

ScrollBar *PlotZoomer::verticalScrollBar() const
{
  return m_vScrollData->m_scrollBar;
}

void PlotZoomer::setHScrollBarMode( Qt::ScrollBarPolicy mode )
{
  if ( m_hScrollData->m_mode != mode )
  {
    m_hScrollData->m_mode = mode;
    updateScrollBars();
  }
}

void PlotZoomer::setVScrollBarMode( Qt::ScrollBarPolicy mode )
{
  if ( m_vScrollData->m_mode != mode )
  {
    m_vScrollData->m_mode = mode;
    updateScrollBars();
  }
}

QWidget *PlotZoomer::cornerWidget() const
{
  return m_cornerWidget;
}

void PlotZoomer::layoutScrollBars( const QRect &rect )
{
    int hPos = xAxis();
    if ( m_hScrollData->m_position == OppositeToScale )
        hPos = oppositeAxis( hPos );

    int vPos = yAxis();
    if ( m_vScrollData->m_position == OppositeToScale )
        vPos = oppositeAxis( vPos );

    ScrollBar *hScrollBar = horizontalScrollBar();
    ScrollBar *vScrollBar = verticalScrollBar();

    const int hdim = hScrollBar ? hScrollBar->extent() : 0;
    const int vdim = vScrollBar ? vScrollBar->extent() : 0;

    if ( hScrollBar && hScrollBar->isVisible() )
    {
        int x = rect.x();
        int y = ( hPos == QwtPlot::xTop )
            ? rect.top() : rect.bottom() - hdim + 1;
        int w = rect.width();

        if ( vScrollBar && vScrollBar->isVisible() )
        {
            if ( vPos == QwtPlot::yLeft )
                x += vdim;
            w -= vdim;
        }

        hScrollBar->setGeometry( x, y, w, hdim );
    }
    if ( vScrollBar && vScrollBar->isVisible() )
    {
        int pos = yAxis();
        if ( m_vScrollData->m_position == OppositeToScale )
            pos = oppositeAxis( pos );

        int x = ( vPos == QwtPlot::yLeft )
            ? rect.left() : rect.right() - vdim + 1;
        int y = rect.y();

        int h = rect.height();

        if ( hScrollBar && hScrollBar->isVisible() )
        {
            if ( hPos == QwtPlot::xTop )
                y += hdim;

            h -= hdim;
        }

        vScrollBar->setGeometry( x, y, vdim, h );
    }
    if ( hScrollBar && hScrollBar->isVisible() &&
        vScrollBar && vScrollBar->isVisible() )
    {
        if ( m_cornerWidget )
        {
            QRect cornerRect(
                vScrollBar->pos().x(), hScrollBar->pos().y(),
                vdim, hdim );
            m_cornerWidget->setGeometry( cornerRect );
        }
    }
}

bool PlotZoomer::needScrollBar( Qt::Orientation orientation ) const
{
  Qt::ScrollBarPolicy mode;
  double zoomMin, zoomMax, baseMin, baseMax;

  if ( orientation == Qt::Horizontal )
  {
    mode = m_hScrollData->m_mode;
    baseMin = zoomBase().left();
    baseMax = zoomBase().right();
    zoomMin = zoomRect().left();
    zoomMax = zoomRect().right();
  }
  else
  {
    mode = m_vScrollData->m_mode;
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

void PlotZoomer::scrollBarMoved(
    Qt::Orientation o, double min, double max )
{
  Q_UNUSED( max );

  if ( o == Qt::Horizontal )
    moveTo( QPointF( min, zoomRect().top() ) );
  else
    moveTo( QPointF( zoomRect().left(), min ) );
      
  Q_EMIT zoomed( zoomRect() );
}

void PlotZoomer::updateScrollBars()
{
    const int xAxis = QwtPlotZoomer::xAxis();
    const int yAxis = QwtPlotZoomer::yAxis();

    int xScrollBarAxis = xAxis;
    if ( m_hScrollData->m_position == OppositeToScale )
        xScrollBarAxis = oppositeAxis( xScrollBarAxis );

    int yScrollBarAxis = yAxis;
    if ( m_vScrollData->m_position == OppositeToScale )
        yScrollBarAxis = oppositeAxis( yScrollBarAxis );


    QwtPlotLayout *layout = plot()->plotLayout();

    bool showHScrollBar = needScrollBar( Qt::Horizontal );
    if ( showHScrollBar )
    {
        ScrollBar *sb = scrollBar( Qt::Horizontal );
        sb->setPalette( plot()->palette() );
        sb->setInverted( !plot()->axisScaleDiv( xAxis ).isIncreasing() );
        sb->setBase( zoomBase().left(), zoomBase().right() );
        sb->moveSlider( zoomRect().left(), zoomRect().right() );

        if ( !sb->isVisibleTo( canvas() ) )
        {
            sb->show();
            layout->setCanvasMargin( layout->canvasMargin( xScrollBarAxis )
                + sb->extent(), xScrollBarAxis );
        }
    }
    else
    {
        if ( horizontalScrollBar() )
        {
            horizontalScrollBar()->hide();
            layout->setCanvasMargin( layout->canvasMargin( xScrollBarAxis )
                - horizontalScrollBar()->extent(), xScrollBarAxis );
        }
    }

    bool showVScrollBar = needScrollBar( Qt::Vertical );
    if ( showVScrollBar )
    {
        ScrollBar *sb = scrollBar( Qt::Vertical );
        sb->setPalette( plot()->palette() );
        sb->setInverted( !plot()->axisScaleDiv( yAxis ).isIncreasing() );
        sb->setBase( zoomBase().top(), zoomBase().bottom() );
        sb->moveSlider( zoomRect().top(), zoomRect().bottom() );

        if ( !sb->isVisibleTo( canvas() ) )
        {
            sb->show();
            layout->setCanvasMargin( layout->canvasMargin( yScrollBarAxis )
                + sb->extent(), yScrollBarAxis );
        }
    }
    else
    {
        if ( verticalScrollBar() )
        {
            verticalScrollBar()->hide();
            layout->setCanvasMargin( layout->canvasMargin( yScrollBarAxis )
                - verticalScrollBar()->extent(), yScrollBarAxis );
        }
    }

    if ( showHScrollBar && showVScrollBar )
    {
        if ( m_cornerWidget == NULL )
        {
            m_cornerWidget = new QWidget( canvas() );
            m_cornerWidget->setAutoFillBackground( true );
            m_cornerWidget->setPalette( plot()->palette() );
        }
        m_cornerWidget->show();
    }
    else
    {
        if ( m_cornerWidget )
            m_cornerWidget->hide();
    }

    layoutScrollBars( canvas()->contentsRect() );
    plot()->updateLayout();
}

void PlotZoomer::widgetMouseDoubleClickEvent(QMouseEvent*)
{
  FractalWidget* fw = (FractalWidget *)plot();
  
  fw->doubleClicked();
  
  reset();
}

