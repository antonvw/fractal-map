////////////////////////////////////////////////////////////////////////////////
// Name:      scrollbar.h
// Purpose:   Declaration of class ScrollBar
// Author:    Anton van Wezenbeek
// Copyright: (c) 2014 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#pragma once 

#include <QScrollBar>

class ScrollBar: public QScrollBar
{
  Q_OBJECT

public:
  ScrollBar( Qt::Orientation, QWidget* parent);

  void setInverted( bool );

  double minBaseValue() const;
  double maxBaseValue() const;

  double minSliderValue() const;
  double maxSliderValue() const;

  int extent() const;

signals:
  void sliderMoved( Qt::Orientation, double, double );
  void valueChanged( Qt::Orientation, double, double );

public slots:
  virtual void setBase( double min, double max );
  virtual void moveSlider( double min, double max );

protected:
  double mapFromTick( int ) const;
  int mapToTick( double ) const;
  void sliderRange( int value, double &min, double &max ) const;

private slots:
  void catchSliderMoved( int value );
  void catchValueChanged( int value );

private:
  int m_baseTicks;
  bool m_inverted;
  double m_maxBase;
  double m_minBase;
};
