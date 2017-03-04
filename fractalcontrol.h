////////////////////////////////////////////////////////////////////////////////
// Name:      fractalcontrol.h
// Purpose:   Declaration of class FractalControl
// Author:    Anton van Wezenbeek
// Copyright: (c) 2017 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QCheckBox>
#include <QColorDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QToolBar>
#include <qwt_interval.h>
#include "fractalgeometry.h"

const QString pointf_regexp("-?[0-9.]+[0-9]*,-?[0-9.]+[0-9]*");
const QString intervals_regexp = pointf_regexp + "," + pointf_regexp;
const QString size_regexp("[1-9][0-9]*,[1-9][0-9]*");

/// This class allows to control geometry values for a fractal.
/// Call addControls with you toolbar, then the right
/// controls will be put on the toolbar. 
class FractalControl : public QObject
{
  Q_OBJECT

public:
  /// Default constructor.
  FractalControl(
    /// using this x interval
    const QwtInterval& xInterval = QwtInterval(-2,2),
    /// using this y interval
    const QwtInterval& yInterval = QwtInterval(-2,2),
    /// iteration depth
    int depth = 0,
    /// using max colours,
    int colours = 0,
    /// dir for images
    const QString& dir = QString());
    
  /// Copy constructor.
  FractalControl(const FractalGeometry& geo);
  
  /// Constructs and adds all edit controls to toolbar.
  void addControls(QToolBar* toolbar);

  /// Returns the geometry.
  auto& geo() {return m_geo;};
  
  /// Returns the geometry.
  const auto& geo() const {return m_geo;};
  
  /// Sets all old colours into new colour.
  void setColours(uint old, uint colour);
  
  /// Sets intervals.
  void setIntervals(const QwtInterval& x, const QwtInterval& y);
signals:
  /// Whenever a control is changed, this signal is emitted.
  void changed();
  
  /// Whenever interval is changed manually, this signal is emitted.
  void changedIntervals();
public slots:  
  /// Sets colours from begin.
  void setColoursDialogBegin() {setColoursDialog(true);};
  
  /// Sets colours from end.
  void setColoursDialogEnd() {setColoursDialog(false);};
  
  /// Sets images.
  void setImages();
private slots:  
  void setColour(const QColor& color);
  void setColoursMinWave(int value);
  void setColoursMax(int value);
  void setColoursMaxWave(int value);
  void setDepth(int value);
  void setImagesSize();
  void setIntervals();
  void setUseImages(int state);
private:  
  void setColours(int colours);
  void setColoursDialog(bool from_start);
  void setImages(bool show_dialog);

  FractalGeometry m_geo;

  QCheckBox* m_useImagesEdit;
  QColorDialog* m_colourDialog;
  QLineEdit *m_imagesSizeEdit, *m_intervalsEdit;
  QSpinBox *m_coloursEdit, *m_coloursMaxWaveEdit,
    *m_coloursMinWaveEdit, *m_depthEdit;
};
