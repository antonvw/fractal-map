////////////////////////////////////////////////////////////////////////////////
// Name:      scrollbar.cpp
// Purpose:   Implementation of class ScrollBar
// Author:    Anton van Wezenbeek
// Copyright: (c) 2014 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#include <QStyle>
#include <QStyleOption>
#include "scrollbar.h"

ScrollBar::ScrollBar( Qt::Orientation o, QWidget *parent )
  : QScrollBar( o, parent )
  , m_baseTicks( 1000000 )
  , m_inverted( orientation() == Qt::Vertical)
  , m_maxBase( 1.0 )
  , m_minBase( 0.0 )
{
  moveSlider( m_minBase, m_maxBase );

  connect( this, SIGNAL( sliderMoved( int ) ), SLOT( catchSliderMoved( int ) ) );
  connect( this, SIGNAL( valueChanged( int ) ), SLOT( catchValueChanged( int ) ) );
}

void ScrollBar::setInverted( bool inverted )
{
  if ( m_inverted != inverted )
  {
    m_inverted = inverted;
    moveSlider( minSliderValue(), maxSliderValue() );
  }
}

void ScrollBar::setBase( double min, double max )
{
  if ( min != m_minBase || max != m_maxBase )
  {
    m_minBase = min;
    m_maxBase = max;

    moveSlider( minSliderValue(), maxSliderValue() );
  }
}

void ScrollBar::moveSlider( double min, double max )
{
  const int sliderTicks = qRound( ( max - min ) /
      ( m_maxBase - m_minBase ) * m_baseTicks );

  // setRange initiates a valueChanged of the scrollbars
  // in some situations. So we block
  // and unblock the signals.

  blockSignals( true );

  setRange( sliderTicks / 2, m_baseTicks - sliderTicks / 2 );
  int steps = sliderTicks / 200;
  if ( steps <= 0 )
      steps = 1;

  setSingleStep( steps );
  setPageStep( sliderTicks );

  int tick = mapToTick( min + ( max - min ) / 2 );
  if ( m_inverted )
      tick = m_baseTicks - tick;

  setSliderPosition( tick );
  blockSignals( false );
}

double ScrollBar::minBaseValue() const
{
  return m_minBase;
}

double ScrollBar::maxBaseValue() const
{
  return m_maxBase;
}

void ScrollBar::sliderRange( int value, double &min, double &max ) const
{
  if ( m_inverted )
      value = m_baseTicks - value;

  const int visibleTicks = pageStep();

  min = mapFromTick( value - visibleTicks / 2 );
  max = mapFromTick( value + visibleTicks / 2 );
}

double ScrollBar::minSliderValue() const
{
  double min, dummy;
  sliderRange( value(), min, dummy );
  return min;
}

double ScrollBar::maxSliderValue() const
{
  double max, dummy;
  sliderRange( value(), dummy, max );
  return max;
}

int ScrollBar::mapToTick( double v ) const
{
  const double pos = ( v - m_minBase ) / ( m_maxBase - m_minBase ) * m_baseTicks;
  return static_cast<int>( pos );
}

double ScrollBar::mapFromTick( int tick ) const
{
  return m_minBase + ( m_maxBase - m_minBase ) * tick / m_baseTicks;
}

void ScrollBar::catchValueChanged( int value )
{
  double min, max;
  
  sliderRange( value, min, max );
  emit valueChanged( orientation(), min, max );
}

void ScrollBar::catchSliderMoved( int value )
{
  double min, max;
  sliderRange( value, min, max );
  Q_EMIT sliderMoved( orientation(), min, max );
}

int ScrollBar::extent() const
{
  QStyleOptionSlider opt;
  opt.init( this );
  opt.subControls = QStyle::SC_None;
  opt.activeSubControls = QStyle::SC_None;
  opt.orientation = orientation();
  opt.minimum = minimum();
  opt.maximum = maximum();
  opt.sliderPosition = sliderPosition();
  opt.sliderValue = value();
  opt.singleStep = singleStep();
  opt.pageStep = pageStep();
  opt.upsideDown = invertedAppearance();
  if ( orientation() == Qt::Horizontal )
      opt.state |= QStyle::State_Horizontal;
      
  return style()->pixelMetric( QStyle::PM_ScrollBarExtent, &opt, this );
}
