////////////////////////////////////////////////////////////////////////////////
// Name:      fractalgeometry.cpp
// Purpose:   Implementation of class FractalGeometry
// Author:    Anton van Wezenbeek
// Copyright: (c) 2014 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#include <QFileDialog>
#include "fractalgeometry.h"

const int min_wave = 380;
const int max_wave = 780;

FractalGeometry::FractalGeometry(
  const QwtInterval& xInterval,
  const QwtInterval& yInterval,
  int firstPass,
  int maxPasses,
  int colours,
  const QString& dir)
  : m_intervalX(xInterval)
  , m_intervalY(yInterval)
  , m_dir(dir)
  , m_imagesSize(32, 32)
  , m_coloursMinWave(min_wave)
  , m_coloursMaxWave(max_wave)
  , m_firstPass(firstPass)
  , m_maxPasses(maxPasses)
  , m_singlePass(false)
  , m_useImages(false)
  , m_colourDialog(new QColorDialog())
{
  setColoursMax(colours);
}

FractalGeometry::FractalGeometry(const FractalGeometry& geo)
{
  *this = geo;
}

FractalGeometry& FractalGeometry::operator= (const FractalGeometry& geo)
{
  m_colourIndexFromStart = geo.m_colourIndexFromStart;
  m_coloursMinWave = geo.m_coloursMinWave;
  m_coloursMaxWave = geo.m_coloursMaxWave;
  m_colourIndex = geo.m_colourIndex;
  m_firstPass = geo.m_firstPass;
  m_maxPasses = geo.m_maxPasses;
  m_singlePass = geo.m_singlePass;
  m_colours = geo.m_colours;
  m_colourDialog = geo.m_colourDialog;
  m_useImages = geo.m_useImages;
  m_images = geo.m_images;
  m_dir = geo.m_dir;
  m_intervalX = geo.m_intervalX;
  m_intervalY = geo.m_intervalY;
  
  return *this;
}

void FractalGeometry::addControls(QToolBar* toolbar)
{
  m_coloursEdit = new QSpinBox();
  m_coloursEdit->setMaximum(8192);
  m_coloursEdit->setMinimum(2);
  m_coloursEdit->setValue(m_colours.size());
  m_coloursEdit->setToolTip("colours");
  
  m_coloursMinWaveEdit = new QSpinBox();
  m_coloursMinWaveEdit->setMinimum(min_wave);
  m_coloursMinWaveEdit->setMaximum(max_wave);
  m_coloursMinWaveEdit->setToolTip("min wavelength colour");
  m_coloursMinWaveEdit->setValue(m_coloursMinWave);
  
  m_coloursMaxWaveEdit = new QSpinBox();
  m_coloursMaxWaveEdit->setMinimum(min_wave);
  m_coloursMaxWaveEdit->setMaximum(max_wave);
  m_coloursMaxWaveEdit->setToolTip("max wavelength colour");
  m_coloursMaxWaveEdit->setValue(m_coloursMaxWave);
  
  m_firstPassEdit = new QSpinBox();
  m_firstPassEdit->setMaximum(32);
  m_firstPassEdit->setMinimum(1);
  m_firstPassEdit->setValue(m_firstPass);
  m_firstPassEdit->setToolTip("first pass");
  
  m_imagesSizeEdit = new QLineEdit();
  m_imagesSizeEdit->setToolTip("images max size");
  m_imagesSizeEdit->setValidator(new QRegExpValidator(QRegExp(size_regexp)));
  m_imagesSizeEdit->setEnabled(false);
  m_imagesSizeEdit->setText(
    QString::number(m_imagesSize.width()) + "," + QString::number(m_imagesSize.height()));

  m_intervalsEdit = new QLineEdit();
  m_intervalsEdit->setToolTip("interval x,y");
  m_intervalsEdit->setValidator(new QRegExpValidator(QRegExp(intervals_regexp)));
  m_intervalsEdit->setText(
    QString::number(m_intervalX.minValue()) + "," + QString::number(m_intervalX.maxValue()) + "," +
    QString::number(m_intervalY.minValue()) + "," + QString::number(m_intervalY.maxValue()));
  
  m_maxPassesEdit = new QSpinBox();
  m_maxPassesEdit->setMaximum(32);
  m_maxPassesEdit->setMinimum(m_firstPassEdit->value());
  m_maxPassesEdit->setValue(m_maxPasses);
  m_maxPassesEdit->setToolTip("last pass");
  
  m_useImagesEdit = new QCheckBox("Images");
  m_useImagesEdit->setToolTip("use images");

  connect(m_colourDialog, SIGNAL(colorSelected(const QColor&)),
    this, SLOT(setColour(const QColor&)));
  connect(m_coloursEdit, SIGNAL(valueChanged(int)),
    this, SLOT(setColoursMax(int)));
  connect(m_coloursMinWaveEdit, SIGNAL(valueChanged(int)),
    this, SLOT(setColoursMinWave(int)));
  connect(m_coloursMaxWaveEdit, SIGNAL(valueChanged(int)),
    this, SLOT(setColoursMaxWave(int)));
  connect(m_firstPassEdit, SIGNAL(valueChanged(int)),
    this, SLOT(setFirstPass(int)));
  connect(m_imagesSizeEdit, SIGNAL(returnPressed()),
    this, SLOT(setImagesSize()));
  connect(m_intervalsEdit, SIGNAL(returnPressed()),
    this, SLOT(setIntervals()));
  connect(m_maxPassesEdit, SIGNAL(valueChanged(int)),
    this, SLOT(setMaxPasses(int)));
  connect(m_useImagesEdit, SIGNAL(stateChanged(int)),
    this, SLOT(setUseImages(int)));
    
  toolbar->addWidget(m_coloursEdit);
  toolbar->addWidget(m_coloursMinWaveEdit);
  toolbar->addWidget(m_coloursMaxWaveEdit);
  toolbar->addSeparator();
  toolbar->addWidget(m_useImagesEdit);
  toolbar->addWidget(m_imagesSizeEdit);
  toolbar->addSeparator();
  toolbar->addWidget(m_firstPassEdit);
  toolbar->addWidget(m_maxPassesEdit);
  toolbar->addSeparator();
  toolbar->addWidget(m_intervalsEdit);
}

bool FractalGeometry::isOk() const
{
  return
     m_firstPass <= m_maxPasses && 
     m_intervalX.isValid() &&
     m_intervalY.isValid() &&
  ((!m_useImages && !m_colours.empty()) || (m_useImages && !m_images.empty()));
}

void FractalGeometry::setColour(const QColor& color)
{
  if (!color.isValid())
  {
    return;
  }

  bool updated = false;
  bool finished = false;
    
  if (m_colours[m_colourIndex] != color.rgb())
  {
    m_colours[m_colourIndex] = color.rgb();  
    updated = true;
  }
  
  if (m_colourIndexFromStart)
  {
    if ((unsigned int)m_colourIndex < m_colours.size() - 1)
    {
      m_colourIndex++;
    }
    else
    {
      finished = true;
    }
  }
  else
  {
    if (m_colourIndex >= 1)
    {
      m_colourIndex--;
    }
    else
    {
      finished = true;
    }
  }
  
  if (!finished)
  {
    if (updated)
    {
      m_singlePass = true;
      emit changed();
    }
    
    m_colourDialog->setCurrentColor(m_colours[m_colourIndex]);
    m_colourDialog->setWindowTitle(
      QString("Select Colour %1 of %2")
        .arg(m_colourIndex + 1).arg(m_colours.size()));
    m_colourDialog->show();
  }
}

void FractalGeometry::setColours(uint old, uint colour)
{
  for (size_t i = 0; i < m_colours.size(); ++i)
  {
    if (m_colours[i] == old)
    {
      m_colours[i] = colour;
    }
  }
  
  m_singlePass = true;
  emit changed();
}

void FractalGeometry::setColours(int colours)
{
  m_colours.clear();
  
  const double visible_min = m_coloursMinWave;
  const double visible_max = m_coloursMaxWave;

  for (int i = 0; i < colours - 1; ++i)
  {
    m_colours.push_back(
      wav2RGB(visible_min + (i * (visible_max - visible_min) / colours)));
  }
  
  m_colours.push_back(qRgb(0, 0, 0));
  
  m_singlePass = false;
  emit changed();
}

void FractalGeometry::setColoursDialog(bool from_start)
{
  m_colourIndexFromStart = from_start;
  
  if (from_start)
  {
    m_colourIndex = 0;
  }
  else
  {
    m_colourIndex = m_colours.size() - 1;
  }
  
  m_colourDialog->setCurrentColor(m_colours[m_colourIndex]);
  m_colourDialog->setWindowTitle(
    QString("Select Colour %1 of %2")
    .arg(m_colourIndex + 1).arg(m_colours.size()));
  m_colourDialog->show();
}
  
void FractalGeometry::setColoursMax(int value)
{
  if (value > 0)
  {
    setColours(value);
  }
}

void FractalGeometry::setColoursMaxWave(int value)
{
  if (value > 0)
  {
    m_coloursMaxWave = value;
    setColours(m_colours.size());
  }
}

void FractalGeometry::setColoursMinWave(int value)
{
  if (value > 0)
  {
    m_coloursMinWave = value;
    setColours(m_colours.size());
  }
}

void FractalGeometry::setFirstPass(int value)
{
  if (value > 0)
  {
    m_maxPassesEdit->setMinimum(value);
    m_firstPass = value;
    m_singlePass = false;
    emit changed();
  }
}

void FractalGeometry::setImages()
{
  setImages(true);
}

void FractalGeometry::setImages(bool show_dialog)
{
  if (show_dialog)
  {
    m_imagesList = QFileDialog::getOpenFileNames(
      NULL,
      "Select Images",
      m_dir.path(),
      "Images (*.bmp *.gif  *.ico *.jpg *.png *.xpm)");
  }

  if (!m_imagesList.isEmpty())
  {
    m_images.clear();
  
    m_dir = m_imagesList[0];
    
    for (int i = 0; i < m_imagesList.size(); i++)
    {
      const QImage image(m_imagesList[i]);
      
      if (
        image.width() > m_imagesSize.width() || 
        image.height() > m_imagesSize.height())
      {
        m_images.push_back(image.scaled(m_imagesSize));
      }
      else
      {
        m_images.push_back(image);
      }
    }
    
    m_singlePass = false;
    emit changed();
  }
}

void FractalGeometry::setImagesSize()
{
  const QStringList sl(m_imagesSizeEdit->text().split(","));
  
  if (sl.size() == 2)
  {
    m_imagesSize = QSize(sl[0].toInt(), sl[1].toInt());

    setImages(m_images.empty());
  }
}

void FractalGeometry::setIntervals()
{
  const QStringList sl(m_intervalsEdit->text().split(","));
  
  if (sl.size() == 4)
  {
    setIntervals(
      QwtInterval(sl[0].toDouble(), sl[1].toDouble()),    
      QwtInterval(sl[2].toDouble(), sl[3].toDouble()));
      
    emit changedIntervals();
  }
}

void FractalGeometry::setIntervals(const QwtInterval& x, const QwtInterval& y)
{
  m_singlePass = false;
  m_intervalX = x;
  m_intervalY = y;
}

void FractalGeometry::setMaxPasses(int value)
{
  if (value > 0)
  {
    m_maxPasses = value;
    m_singlePass = true;
    emit changed();
  }
}

void FractalGeometry::setUseImages(int state)
{
  const bool use = (state == Qt::Checked);
  m_imagesSizeEdit->setEnabled(use);  
  
  if (use && m_images.empty())
  {
    setImages();
    
    if (m_images.empty())
    {
      return;
    }
  }
  
  m_useImages = use;
  
  m_coloursEdit->setEnabled(!m_useImages);
  m_coloursMaxWaveEdit->setEnabled(!m_useImages);
  m_coloursMinWaveEdit->setEnabled(!m_useImages);

  m_singlePass = false;
  emit changed();
}

// see
// http://codingmess.blogspot.com/2009/05/conversion-of-wavelength-in-nanometers.html
uint FractalGeometry::wav2RGB(double w) const
{
  double R = 0.0;
  double G = 0.0;
  double B = 0.0;
  
  if (w >= min_wave && w < 440)
  {
    R = -(w - 440) / (440 - 350);
    G = 0.0;
    B = 1.0;
  }
  else if (w >= 440 && w < 490)
  {
    R = 0.0;
    G = (w - 440) / (490 - 440);
    B = 1.0;
  }
  else if (w >= 490 && w < 510)
  {
    R = 0.0;
    G = 1.0;
    B = -(w - 510) / (510 - 490);
  }
  else if (w >= 510 && w < 580)
  {
    R = (w - 510) / (580 - 510);
    G = 1.0;
    B = 0.0;
  }
  else if (w >= 580 && w < 645)
  {
    R = 1.0;
    G = -(w - 645) / (645 - 580);
    B = 0.0;
  }
  else if (w >= 645 && w <= max_wave)
  {
    R = 1.0;
    G = 0.0;
    B = 0.0;
  }

  // intensity correction
  double SSS = 0;
  
  if (w >= min_wave && w < 420)
    SSS = 0.3 + 0.7*(w - 350) / (420 - 350);
  else if (w >= 420 && w <= 700)
    SSS = 1.0;
  else if (w > 700 && w <= max_wave)
    SSS = 0.3 + 0.7*(max_wave - w) / (max_wave - 700);
      
  SSS *= 255;

  return qRgb(int(SSS*R), int(SSS*G), int(SSS*B));
}
