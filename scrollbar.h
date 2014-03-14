////////////////////////////////////////////////////////////////////////////////
// Name:      scrollbar.h
// Purpose:   Declaration of class ScrollBar
// Author:    Anton van Wezenbeek
// Copyright: (c) 2014 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#pragma once 

#include <QScrollBar>

/// This class offers a scrollbar to be used on the plot.
class ScrollBar: public QScrollBar
{
  Q_OBJECT

public:
  enum ScrollBarPosition
  {
    AttachedToScale,
    OppositeToScale
  };

  /// Constructor.
  ScrollBar( ScrollBarPosition pos, Qt::Orientation, QWidget* parent);

  int extent() const;

  /// Access to members.
  double minBaseValue() const {return m_minBase;};
  double minSliderValue() const;
  double maxBaseValue() const {return m_maxBase;};
  double maxSliderValue() const;
  Qt::ScrollBarPolicy mode() const {return m_mode;};
  ScrollBarPosition position() const {return m_position;};

  void moveSlider( double min, double max );
  void setBase( double min, double max );

signals:
  void sliderMoved( Qt::Orientation, double, double );
  void valueChanged( Qt::Orientation, double, double );

private slots:
  void catchSliderMoved( int value );
  void catchValueChanged( int value );

private:
  double mapFromTick( int ) const;
  int mapToTick( double ) const;
  void sliderRange( int value, double &min, double &max ) const;

  const int m_baseTicks;
  const bool m_inverted;
  const Qt::ScrollBarPolicy m_mode;
  const ScrollBarPosition m_position;
  
  double m_maxBase;
  double m_minBase;
};
