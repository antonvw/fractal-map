////////////////////////////////////////////////////////////////////////////////
// Name:      fractalgeometry.h
// Purpose:   Declaration of class FractalGeometry
// Author:    Anton van Wezenbeek
// Copyright: (c) 2012 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#ifndef _FRACTALGEOMETRY_H
#define _FRACTALGEOMETRY_H

#include <vector>
#include <QCheckBox>
#include <QColorDialog>
#include <QDir>
#include <QLineEdit>
#include <QPointF>
#include <QSpinBox>
#include <QToolBar>

const QString pointf_regexp("-?[0-9.]+[0-9]*,-?[0-9.]+[0-9]*");
const QString size_regexp("[0-9]+,[0-9]+");

/// This class contains general geometry values for a fractal.
/// If you want to edit values supplied using the default constructor,
/// call addControls with you toolbar, then the right
/// controls will be put on the toolbar. 
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
    int colours = 0,
    /// dir for images
    const QString& dir = QString());
    
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
  
  /// Gets the dir used when using images instead fo colours.
  const QDir& dir() const {return m_dir;};
  
  /// Gets first pass.
  int firstPass() const {return m_firstPass;};
  
  /// Gets images.
  const std::vector<QImage> & images() const {return m_images;};
  
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
  
  /// Sets all old colours into new colour.
  void setColours(uint old, uint colour);
  
  /// Gets single pass.
  bool singlePass() const {return m_singlePass;};
  
  /// Gets use images.
  bool useImages() const {return m_useImages;};

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
  
  /// Sets images.
  void setImages();
private slots:  
  void setCenter();
  void setColour(const QColor& color);
  void setColoursMinWave(int value);
  void setColoursMax(int value);
  void setColoursMaxWave(int value);
  void setFirstPass(int value);
  void setImagesSize();
  void setMaxPasses(int value);
  void setScale(const QString& text);
  void setUseImages(int state);
private:  
  void setColours(int colours);
  void setColoursDialog(bool from_start);
  uint wav2RGB(double wave) const;
  
  QLineEdit* m_centerEdit;
  QSpinBox* m_coloursEdit;
  QSpinBox* m_coloursMaxWaveEdit;
  QSpinBox* m_coloursMinWaveEdit;
  QSpinBox* m_firstPassEdit;
  QLineEdit* m_imagesSizeEdit;
  QSpinBox* m_maxPassesEdit;
  QLineEdit* m_scaleEdit;
  QCheckBox* m_useImagesEdit;
  
  bool m_colourIndexFromStart;
  
  QPointF m_center;
  QDir m_dir;
  QSize m_imagesSize;
  QPoint m_origin;

  double m_coloursMinWave;  
  double m_coloursMaxWave;  
  double m_scale;
  
  int m_colourIndex;
  int m_firstPass;
  int m_maxPasses;
  
  bool m_singlePass;
  // use images instead of colours for rendering
  bool m_useImages;
  
  // the last colour is used for converge
  std::vector<uint> m_colours;
  
  // the last image is used for convergence
  std::vector<QImage> m_images;
  
  QColorDialog* m_colourDialog;
};
#endif
