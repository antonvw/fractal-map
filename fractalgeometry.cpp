////////////////////////////////////////////////////////////////////////////////
// Name:      fractalgeometry.cpp
// Purpose:   Implementation of class FractalGeometry
// Author:    Anton van Wezenbeek
// Copyright: (c) 2012 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#include <QFileDialog>
#include "fractalgeometry.h"

FractalGeometry::FractalGeometry(
  const QPointF& center,
  double scale,
  int firstPass,
  int maxPasses,
  int colours,
  const QString& dir)
  : m_center(center)
  , m_dir(dir)
  , m_imagesSize(32, 32)
  , m_origin(0, 0)
  , m_coloursMinWave(380)
  , m_coloursMaxWave(780)
  , m_scale(scale)
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
  m_center = geo.m_center;
  m_origin = geo.m_origin;
  m_coloursMinWave = geo.m_coloursMinWave;
  m_coloursMaxWave = geo.m_coloursMaxWave;
  m_scale = geo.m_scale;
  m_colourIndex = geo.m_colourIndex;
  m_firstPass = geo.m_firstPass;
  m_maxPasses = geo.m_maxPasses;
  m_singlePass = geo.m_singlePass;
  m_colours = geo.m_colours;
  m_colourDialog = geo.m_colourDialog;
  m_useImages = geo.m_useImages;
  m_images = geo.m_images;
  m_dir = geo.m_dir;
  
  return *this;
}

void FractalGeometry::addControls(QToolBar* toolbar)
{
  m_centerEdit = new QLineEdit();
  m_centerEdit->setText(
    QString::number(m_center.x()) + "," + QString::number(m_center.y()));
  m_centerEdit->setToolTip("center x,y");
  m_centerEdit->setValidator(new QRegExpValidator(QRegExp(pointf_regexp)));
  
  m_coloursEdit = new QSpinBox();
  m_coloursEdit->setMaximum(8192);
  m_coloursEdit->setValue(m_colours.size());
  m_coloursEdit->setToolTip("colours");
  
  m_coloursMinWaveEdit = new QSpinBox();
  m_coloursMinWaveEdit->setMinimum(380);
  m_coloursMinWaveEdit->setMaximum(780);
  m_coloursMinWaveEdit->setToolTip("min wavelength colour");
  m_coloursMinWaveEdit->setValue(m_coloursMinWave);
  
  m_coloursMaxWaveEdit = new QSpinBox();
  m_coloursMaxWaveEdit->setMinimum(380);
  m_coloursMaxWaveEdit->setMaximum(780);
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

  m_maxPassesEdit = new QSpinBox();
  m_maxPassesEdit->setMaximum(32);
  m_maxPassesEdit->setMinimum(m_firstPassEdit->value());
  m_maxPassesEdit->setValue(m_maxPasses);
  m_maxPassesEdit->setToolTip("last pass");
  
  m_scaleEdit = new QLineEdit();
  m_scaleEdit->setText(QString::number(m_scale));
  m_scaleEdit->setValidator(new QDoubleValidator());
  m_scaleEdit->setToolTip("scale");
  
  m_useImagesEdit = new QCheckBox("Images");
  m_useImagesEdit->setToolTip("use images");

  connect(m_centerEdit, SIGNAL(returnPressed()),
    this, SLOT(setCenter()));
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
  connect(m_maxPassesEdit, SIGNAL(valueChanged(int)),
    this, SLOT(setMaxPasses(int)));
  connect(m_scaleEdit, SIGNAL(textEdited(const QString&)),
    this, SLOT(setScale(const QString&)));
  connect(m_useImagesEdit, SIGNAL(stateChanged(int)),
    this, SLOT(setUseImages(int)));
    
  toolbar->addWidget(m_firstPassEdit);
  toolbar->addWidget(m_maxPassesEdit);
  toolbar->addSeparator();
  toolbar->addWidget(m_coloursEdit);
  toolbar->addWidget(m_coloursMinWaveEdit);
  toolbar->addWidget(m_coloursMaxWaveEdit);
  toolbar->addSeparator();
  toolbar->addWidget(m_centerEdit);
  toolbar->addWidget(m_scaleEdit);
  toolbar->addWidget(m_useImagesEdit);
  toolbar->addWidget(m_imagesSizeEdit);
}

bool FractalGeometry::isOk() const
{
  return
     m_firstPass <= m_maxPasses && 
  ((!m_useImages && !m_colours.empty()) || (m_useImages && !m_images.empty())) && 
     m_scale > 0;
}

void FractalGeometry::scroll(const QPoint& delta)
{
  m_singlePass = false;
  m_center -= QPointF(delta) * scale();
  m_origin += delta;
  m_centerEdit->setText(
    QString::number(m_center.x()) + "," + QString::number(m_center.y()));
  
  emit changed();
}

void FractalGeometry::setCenter()
{
  const QStringList sl(m_centerEdit->text().split(","));
  
  if (sl.size() == 2)
  {
    m_singlePass = false;
    m_center = QPointF(
      sl[0].toDouble(),
      sl[1].toDouble());
      
    emit changed();
  }
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
  const QStringList sl = QFileDialog::getOpenFileNames(
    NULL,
    "Select Images",
    m_dir.path(),
    "Images (*.bmp *.gif  *.ico *.jpg *.png *.xpm)");

  if (!sl.isEmpty())
  {
    m_images.clear();
  
    m_dir = sl[0];
    
    for (int i = 0; i < sl.size(); i++)
    {
      const QImage image(sl[i]);
      
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
    
    setImages();
  }
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

void FractalGeometry::setScale(const QString& text)
{
  if (text.isEmpty())
  {
    return;
  }
  
  const double scale = text.toDouble();
  
  if (scale != 0)
  {
    m_scale = scale;
    m_singlePass = false;
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
  
  if (w >= 380 && w < 440)
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
  else if (w >= 645 && w <= 780)
  {
    R = 1.0;
    G = 0.0;
    B = 0.0;
  }

  // intensity correction
  double SSS = 0;
  
  if (w >= 380 && w < 420)
    SSS = 0.3 + 0.7*(w - 350) / (420 - 350);
  else if (w >= 420 && w <= 700)
    SSS = 1.0;
  else if (w > 700 && w <= 780)
    SSS = 0.3 + 0.7*(780 - w) / (780 - 700);
      
  SSS *= 255;

  return qRgb(int(SSS*R), int(SSS*G), int(SSS*B));
}

void FractalGeometry::zoom(double zoomFactor)
{
  m_scale *= zoomFactor;
  m_center *= zoomFactor;
  m_scaleEdit->setText(QString::number(m_scale));
  m_singlePass = false;
  
  emit changed();
}
