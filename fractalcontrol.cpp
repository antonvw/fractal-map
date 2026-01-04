////////////////////////////////////////////////////////////////////////////////
// Name:      fractalcontrol.cpp
// Purpose:   Implementation of class FractalControl
// Author:    Anton van Wezenbeek
// Copyright: (c) 2017-2026 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#include <QFileDialog>

#include "fractalcontrol.h"
#include "fractalwidget.h"

FractalControl::FractalControl(
  const QwtInterval& xInterval,
  const QwtInterval& yInterval,
  int depth,
  int colours,
  const QString& dir)
  : m_geo(xInterval, yInterval, depth, dir)
  , m_colourDialog(new QColorDialog())
{
  setColoursMax(colours);
}

FractalControl::FractalControl(const FractalGeometry& geo)
  : QObject()
  , m_geo(geo)
  , m_colourDialog(new QColorDialog())
{
}

void FractalControl::addControls(QToolBar* toolbar)
{
  m_coloursEdit = new QSpinBox();
  m_coloursEdit->setMaximum(8192);
  m_coloursEdit->setMinimum(2);
  m_coloursEdit->setValue(m_geo.colours().size());
  m_coloursEdit->setToolTip("colours");
  
  m_coloursMinWaveEdit = new QSpinBox();
  m_coloursMinWaveEdit->setMinimum(min_wave);
  m_coloursMinWaveEdit->setMaximum(max_wave);
  m_coloursMinWaveEdit->setToolTip("min wavelength colour");
  m_coloursMinWaveEdit->setValue(m_geo.m_coloursMinWave);
  
  m_coloursMaxWaveEdit = new QSpinBox();
  m_coloursMaxWaveEdit->setMinimum(min_wave);
  m_coloursMaxWaveEdit->setMaximum(max_wave);
  m_coloursMaxWaveEdit->setToolTip("max wavelength colour");
  m_coloursMaxWaveEdit->setValue(m_geo.m_coloursMaxWave);
  
  m_depthEdit = new QSpinBox();
  m_depthEdit->setRange(1, 999999);
  m_depthEdit->setValue(m_geo.m_depth);
  m_depthEdit->setToolTip("depth");
  
  m_imagesSizeEdit = new QLineEdit();
  m_imagesSizeEdit->setToolTip("images max size");
  m_imagesSizeEdit->setValidator(new QRegularExpressionValidator(QRegularExpression(size_regexp)));
  m_imagesSizeEdit->setEnabled(false);
  m_imagesSizeEdit->setText(
    QString::number(m_geo.m_imagesSize.width()) + "," + QString::number(m_geo.m_imagesSize.height()));

  m_intervalsEdit = new QLineEdit();
  m_intervalsEdit->setToolTip("interval x,y");
  m_intervalsEdit->setValidator(new QRegularExpressionValidator(QRegularExpression(intervals_regexp)));
  m_intervalsEdit->setText(
    QString::number(m_geo.m_intervalX.minValue()) + "," + QString::number(m_geo.m_intervalX.maxValue()) + "," +
    QString::number(m_geo.m_intervalY.minValue()) + "," + QString::number(m_geo.m_intervalY.maxValue()));
  
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
  connect(m_depthEdit, SIGNAL(valueChanged(int)),
    this, SLOT(setDepth(int)));
  connect(m_imagesSizeEdit, SIGNAL(returnPressed()),
    this, SLOT(setImagesSize()));
  connect(m_intervalsEdit, SIGNAL(returnPressed()),
    this, SLOT(setIntervals()));
  connect(m_useImagesEdit, SIGNAL(stateChanged(int)),
    this, SLOT(setUseImages(int)));
    
  toolbar->addWidget(m_depthEdit);
  toolbar->addWidget(m_intervalsEdit);
  toolbar->addSeparator();
  toolbar->addWidget(m_coloursEdit);
  toolbar->addWidget(m_coloursMinWaveEdit);
  toolbar->addWidget(m_coloursMaxWaveEdit);
  toolbar->addSeparator();
  toolbar->addWidget(m_useImagesEdit);
  toolbar->addWidget(m_imagesSizeEdit);
}

void FractalControl::setColour(const QColor& color)
{
  if (m_geo.setColour(color))
  {
    emit changed();
    
    m_colourDialog->setCurrentColor(m_geo.colour());
    m_colourDialog->setWindowTitle(
      QString("Select Colour %1 of %2")
        .arg(m_geo.colourIndex() + 1).arg(m_geo.colours().size()));
    m_colourDialog->show();
  }
}

void FractalControl::setColours(uint old, uint colour)
{
  for (auto & it : m_geo.m_colours)
  {
    if (it == old)
    {
      it = colour;
    }
  }
  
  emit changed();
}

void FractalControl::setColours(int colours)
{
  m_geo.setColours(colours);
  emit changed();
}

void FractalControl::setColoursDialog(bool from_start)
{
  m_geo.prepare(from_start);

  m_colourDialog->setCurrentColor(m_geo.colour());
  m_colourDialog->setWindowTitle(
    QString("Select Colour %1 of %2")
    .arg(m_geo.colourIndex() + 1).arg(m_geo.colours().size()));
  m_colourDialog->show();
}
  
void FractalControl::setColoursMax(int value)
{
  if (value > 0)
  {
    setColours(value);
  }
}

void FractalControl::setColoursMaxWave(int value)
{
  if (value > 0)
  {
    m_geo.m_coloursMaxWave = value;
    setColours(m_geo.m_colours.size());
  }
}

void FractalControl::setColoursMinWave(int value)
{
  if (value > 0)
  {
    m_geo.m_coloursMinWave = value;
    setColours(m_geo.m_colours.size());
  }
}

void FractalControl::setDepth(int value)
{
  if (value > 0)
  {
    m_geo.m_depth = value;
    emit changed();
  }
}

void FractalControl::setImages()
{
  setImages(true);
}

void FractalControl::setImages(bool show_dialog)
{
  if (show_dialog)
  {
    m_geo.m_imagesList = QFileDialog::getOpenFileNames(
      nullptr,
      "Select Images",
      m_geo.m_dir.path(),
      "Images (*.bmp *.gif  *.ico *.jpg *.png *.xpm)");
  }

  if (!m_geo.m_imagesList.isEmpty())
  {
    m_geo.m_images.clear();
    m_geo.m_dir = m_geo.m_imagesList[0];
    
    for (const auto & i : m_geo.m_imagesList)
    {
      const QImage image(i);
      
      m_geo.m_images.push_back(
        (image.width() > m_geo.m_imagesSize.width() || 
         image.height() > m_geo.m_imagesSize.height()) ?
        image.scaled(m_geo.m_imagesSize): image);
    }
    
    emit changed();
  }
}

void FractalControl::setImagesSize()
{
  const QStringList sl(m_imagesSizeEdit->text().split(","));
  
  if (sl.size() == 2)
  {
    m_geo.m_imagesSize = QSize(sl[0].toInt(), sl[1].toInt());
    setImages(m_geo.m_images.empty());
  }
}

void FractalControl::setIntervals()
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

void FractalControl::setIntervals(const QwtInterval& x, const QwtInterval& y)
{
  m_geo.m_intervalX = x;
  m_geo.m_intervalY = y;
}

void FractalControl::setUseImages(int state)
{
  const bool use = (state == Qt::Checked);
  m_imagesSizeEdit->setEnabled(use);  
  
  if (use && m_geo.m_images.empty())
  {
    setImages();
    
    if (m_geo.m_images.empty())
    {
      return;
    }
  }
  
  m_geo.m_useImages = use;
  
  m_coloursEdit->setEnabled(!m_geo.m_useImages);
  m_coloursMaxWaveEdit->setEnabled(!m_geo.m_useImages);
  m_coloursMinWaveEdit->setEnabled(!m_geo.m_useImages);

  emit changed();
}
