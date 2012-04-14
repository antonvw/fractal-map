////////////////////////////////////////////////////////////////////////////////
// Name:      fractalgeometry.h
// Purpose:   Declaration of class FractalGeometry
// Author:    Anton van Wezenbeek
// Copyright: (c) 2012 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#ifndef _FRACTALGEOMETRY_H
#define _FRACTALGEOMETRY_H

#include <vector>
#include <QColorDialog>
#include <QLineEdit>
#include <QPointF>
#include <QSpinBox>
#include <QToolBar>

const QString point_regexp("-?[0-9.]+[0-9]*,-?[0-9.]+[0-9]*");

/// This class contains general geometry values for a fractal.
class FractalGeometry : public QObject
{
  Q_OBJECT

public:
  /// Default constructor.
  FractalGeometry(
    /// using this center
    const QPointF& center = QPointF(0,0),
    /// using this scale
    double scale = 0,
    /// pass to start with
    int firstPass = 0,
    /// using max number of passes
    int maxPasses = 0,
    /// using max colours,
    int colours = 0);
    
  /// Copy constructor.
  FractalGeometry(const FractalGeometry& geo);
  
  /// Assignment.
  FractalGeometry& operator= (const FractalGeometry& geo);
  
  /// Constructs and adds all edit controls to toolbar.
  void addControls(QToolBar* toolbar);
  /// Gets the center.
  const QPointF& center() const {return m_center;};
  
  /// Gets colours.
  const std::vector<uint> & colours() const {return m_colours;};
  
  /// Gets first pass.
  int firstPass() const {return m_firstPass;};
  
  /// Returns true if parameters are ok.
  bool isOk() const;
  
  /// Gets max passes.
  int maxPasses() const {return m_maxPasses;};
  
  /// Gets origin.
  const QPoint& origin() const {return m_origin;};
  
  /// Gets scale.
  double scale() const {return m_scale;};
  
  /// Scrolls.
  void scroll(const QPoint& delta);
  
  /// Gets single pass.
  bool singlePass() const {return m_singlePass;};
  
  /// Sets all old colours into new colour.
  void setColours(uint old, uint colour);

  /// Zoom in or out.
  void zoom(double factor);
signals:
  /// Whenever a control is changed, this signal is emitted.
  void changed();
public slots:  
  /// Sets colours from begin.
  void setColoursDialogBegin() {setColoursDialog(true);};
  
  /// Sets colours from end.
  void setColoursDialogEnd() {setColoursDialog(false);};
private slots:  
  void setCenter();
  void setColour(const QColor& color);
  void setColoursMinWave(int value);
  void setColoursMax(int value);
  void setColoursMaxWave(int value);
  void setFirstPass(int value);
  void setMaxPasses(int value);
  void setScale(const QString& text);
private:  
  void setColours(int colours);
  void setColoursDialog(bool from_start);
  uint wav2RGB(double wave) const;
  
  QLineEdit* m_centerEdit;
  QSpinBox* m_coloursEdit;
  QSpinBox* m_coloursMaxWaveEdit;
  QSpinBox* m_coloursMinWaveEdit;
  QSpinBox* m_firstPassEdit;
  QSpinBox* m_maxPassesEdit;
  QLineEdit* m_scaleEdit;

  bool m_colourIndexFromStart;
  
  QPointF m_center;
  QPoint m_origin;

  double m_coloursMinWave;  
  double m_coloursMaxWave;  
  double m_scale;
  
  int m_colourIndex;
  int m_firstPass;
  int m_maxPasses;
  
  bool m_singlePass;
  
  // the last colour is used for converge
  std::vector<uint> m_colours;
  
  QColorDialog* m_colourDialog;
};
#endif
