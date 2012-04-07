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

enum
{
  CHANGED_START,
  CHANGED_CONTINUE,
  CHANGED_FINISH,
};

const QString point_regexp("-?[0-9.]+[0-9]*,-?[0-9.]+[0-9]*");

// This class contains general geometry values for a fractal.
class FractalGeometry : public QObject
{
  Q_OBJECT

public:
  // Default constructor.
  FractalGeometry(
    // using this center
    const QPointF& center = QPointF(0,0),
    // using this scale
    double scale = 0,
    // pass to start with
    int firstPass = 0,
    // using max number of passes
    int maxPasses = 0,
    // using these colours,
    // the last colour is used for converge
    const std::vector<uint> & colours = std::vector<uint>());
    
  // Copy constructor.
  FractalGeometry(const FractalGeometry& geo);
  
  // Assignment.
  FractalGeometry& operator= (const FractalGeometry& geo);
  
  // Constructs and adds all edit controls to toolbar.
  void addControls(QToolBar* toolbar);
    
  // Returns true if parameters are ok.
  bool isOk() const;
  
  // Scroll.
  void scroll(const QPoint& delta);

  // Access to members.
  const QPointF& center() const {return m_center;};
  const std::vector<uint> & colours() const {return m_colours;};
  int firstPass() const {return m_firstPass;};
  int maxPasses() const {return m_maxPasses;};
  const QPoint& origin() const {return m_origin;};
  double scale() const {return m_scale;};
signals:
  // Whenever a control is changed, this signal is emitted.
  void changed(int);
public slots:  
  void setCenter();
  void setColoursMax(int value);
  void setColoursMinWave(int value);
  void setColoursMaxWave(int value);
  void setColourSelected(const QColor& color);
  void setFirstPass(int value);
  void setPasses(int value);
  void setScale(const QString& text);
  void zoom(double zoomFactor);

  // Sets colours.
  void setColoursDialogBegin() {setColoursDialog(true);};
  void setColoursDialogEnd() {setColoursDialog(false);};
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
  
  std::vector<uint> m_colours;
  
  QColorDialog* m_colourDialog;
};
#endif
