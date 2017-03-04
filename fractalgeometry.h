////////////////////////////////////////////////////////////////////////////////
// Name:      fractalgeometry.h
// Purpose:   Declaration of class FractalGeometry
// Author:    Anton van Wezenbeek
// Copyright: (c) 2017 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <QDir>
#include <QImage>
#include <QSize>
#include <qwt_interval.h>

const int min_wave = 380;
const int max_wave = 780;

class FractalControl;

/// This class contains general geometry values for a fractal.
class FractalGeometry
{
  friend class FractalControl;
public:
  /// Default constructor.
  FractalGeometry(
    /// using this x interval
    const QwtInterval& xInterval = QwtInterval(-2,2),
    /// using this y interval
    const QwtInterval& yInterval = QwtInterval(-2,2),
    /// iteration depth
    int depth = 2,
    /// dir for images
    const QString& dir = QString());

  /// Returns current colour.
  const auto & colour() const {return m_colours[m_colourIndex];};

  /// Returns colour.
  const auto & colour(int i) const {return m_colours[i];};

  /// Returns colour index.
  auto colourIndex() const {return m_colourIndex;};
  
  /// Gets colours.
  const auto & colours() const {return m_colours;};

  /// Gets iteration depth.
  auto depth() const {return m_depth;};
  
  /// Gets the dir used when using images instead of colours.
  const auto & dir() const {return m_dir;};

  /// Returns true if finished setColour.
  bool finished() const {return m_finished;};
  
  /// Gets image.
  const auto & image(int i) const {return m_images[i];};
  
  /// Gets images.
  const auto & images() const {return m_images;};
  
  /// Gets the x interval.
  const auto & intervalX() const {return m_intervalX;};
  
  /// Gets the y interval.
  const auto & intervalY() const {return m_intervalY;};
  
  /// Returns true if parameters are ok.
  bool isOk() const;
  
  /// Prepares colour index to be used from start or from end.  
  void prepare(bool from_start) {
    m_colourIndex = (from_start ? 0: m_colours.size() - 1);
    m_colourIndexFromStart = from_start;};

  /// Sets colour(s).
  bool setColour(const QColor& color);

  /// Sets colours.
  void setColours(int size);

  /// Gets use images.
  bool useImages() const {return m_useImages;};
private:
  uint wav2RGB(double wave) const;

  QwtInterval m_intervalX;
  QwtInterval m_intervalY;

  QDir m_dir;
  QSize m_imagesSize = QSize(32, 32);
  QStringList m_imagesList;

  double m_coloursMinWave;  
  double m_coloursMaxWave;  
  
  int m_colourIndex = 0;
  int m_depth;
  
  bool m_colourIndexFromStart = true;
  bool m_finished = false;
  // use images instead of colours for rendering
  bool m_useImages = false;
  
  // the last colour is used for converge
  std::vector<uint> m_colours;
  
  // the last image is used for convergence
  std::vector<QImage> m_images;
};
