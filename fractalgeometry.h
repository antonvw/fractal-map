////////////////////////////////////////////////////////////////////////////////
// Name:      fractalgeometry.h
// Purpose:   Declaration of class FractalGeometry
// Author:    Anton van Wezenbeek
// Copyright: (c) 2015 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <QCheckBox>
#include <QColorDialog>
#include <QDir>
#include <QLineEdit>
#include <QSpinBox>
#include <QToolBar>
#include <qwt_interval.h>

const QString pointf_regexp("-?[0-9.]+[0-9]*,-?[0-9.]+[0-9]*");
const QString intervals_regexp = pointf_regexp + "," + pointf_regexp;
const QString size_regexp("[1-9][0-9]*,[1-9][0-9]*");

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
    /// using this x interval
    const QwtInterval& xInterval = QwtInterval(-2,2),
    /// using this y interval
    const QwtInterval& yInterval = QwtInterval(-2,2),
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
  
  /// Gets colours.
  const std::vector<uint> & colours() const {return m_colours;};
  
  /// Gets the dir used when using images instead of colours.
  const QDir& dir() const {return m_dir;};
  
  /// Gets first pass.
  int firstPass() const {return m_firstPass;};
  
  /// Gets the x interval.
  const QwtInterval& intervalX() const {return m_intervalX;};
  
  /// Gets the y interval.
  const QwtInterval& intervalY() const {return m_intervalY;};
  
  /// Gets images.
  const std::vector<QImage> & images() const {return m_images;};
  
  /// Returns true if parameters are ok.
  bool isOk() const;
  
  /// Gets max passes.
  int maxPasses() const {return m_maxPasses;};
  
  /// Sets all old colours into new colour.
  void setColours(uint old, uint colour);
  
  /// Sets intervals.
  void setIntervals(const QwtInterval& x, const QwtInterval& y);
  
  /// Sets single pass (for auto zooming).
  void setSinglePass() {m_singlePass = true;};
  
  /// Gets single pass.
  bool singlePass() const {return m_singlePass;};
  
  /// Gets use images.
  bool useImages() const {return m_useImages;};
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
  void setFirstPass(int value);
  void setImagesSize();
  void setIntervals();
  void setMaxPasses(int value);
  void setUseImages(int state);
private:  
  void setColours(int colours);
  void setColoursDialog(bool from_start);
  void setImages(bool show_dialog);
  uint wav2RGB(double wave) const;
  
  QSpinBox* m_coloursEdit;
  QSpinBox* m_coloursMaxWaveEdit;
  QSpinBox* m_coloursMinWaveEdit;
  QSpinBox* m_firstPassEdit;
  QLineEdit* m_imagesSizeEdit;
  QLineEdit* m_intervalsEdit;
  QSpinBox* m_maxPassesEdit;
  QCheckBox* m_useImagesEdit;
  
  bool m_colourIndexFromStart;
  
  QwtInterval m_intervalX;
  QwtInterval m_intervalY;
  QDir m_dir;
  QSize m_imagesSize;
  QStringList m_imagesList;

  double m_coloursMinWave;  
  double m_coloursMaxWave;  
  
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
