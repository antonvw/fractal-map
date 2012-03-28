////////////////////////////////////////////////////////////////////////////////
// Name:      fractalwidget.cpp
// Purpose:   Implementation of class FractalWidget
// Author:    Anton van Wezenbeek
// Copyright: (c) 2012 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <QtGui>
#include "fractalwidget.h"
#include "fractal.h"

const double zoomIn = 0.8;

FractalWidget::FractalWidget(
  QWidget* parent,
  QStatusBar* statusbar,
  const QString& fractalName,
  double scale,
  uint colours,
  double diverge,
  const QPointF& center,
  uint first_pass,
  uint passes,
  double julia_real,
  double julia_imag,
  double julia_exponent)
  : QWidget(parent)
  , m_origin(0, 0)
  , m_center(center)
  , m_fractalName(fractalName)
  , m_coloursMinWave(380)
  , m_coloursMaxWave(780)
  , m_diverge(diverge)
  , m_juliaExponent(julia_exponent)
  , m_pixmapScale(scale)
  , m_scale(scale)
  , m_julia(std::complex<double>(julia_real, julia_imag))
  , m_firstPass(first_pass)
  , m_pass(first_pass)
  , m_passes(passes)
  , m_updates(0)
  , m_colourDialog(new QColorDialog())
  , m_juliaToolBar(NULL)
  , m_statusBar(statusbar)
{
  setColoursMax(colours);
  
  init();
}

FractalWidget::FractalWidget(
  const FractalWidget& fractal, QStatusBar* statusbar)
  : QWidget(fractal.parentWidget())
  , m_origin(fractal.m_origin)
  , m_center(fractal.m_center)
  , m_colours(fractal.m_colours)
  , m_fractalName(fractal.m_fractalName)
  , m_coloursMinWave(fractal.m_coloursMinWave)
  , m_coloursMaxWave(fractal.m_coloursMaxWave)
  , m_diverge(fractal.m_diverge)
  , m_juliaExponent(fractal.m_juliaExponent)
  , m_pixmapScale(fractal.m_scale)
  , m_scale(fractal.m_scale)
  , m_julia(fractal.m_julia)
  , m_firstPass(fractal.m_firstPass)
  , m_pass(fractal.m_firstPass)
  , m_passes(fractal.m_passes)
  , m_updates(0)
  , m_colourDialog(fractal.m_colourDialog)
  , m_juliaToolBar(fractal.m_juliaToolBar)
  , m_statusBar(statusbar)
{
  init();
  
  const QImage image(fractal.m_pixmap.toImage());
  
  if (!image.isNull())
  { 
    m_pixmap = QPixmap::fromImage(image);
    update();
  }
}

void FractalWidget::addAxes(QPainter& painter)
{
  if (!m_axesEdit->isChecked())
  {
    return;
  }
  
  const QPoint y1 = QPoint(m_origin.x() + m_pixmap.width() / 2, 0);
  const QPoint y2 = QPoint(m_origin.x() + m_pixmap.width() / 2, m_pixmap.height());
  const QPoint x1 = QPoint(0, m_origin.y() + m_pixmap.height() / 2);
  const QPoint x2 = QPoint(m_pixmap.width(), m_origin.y() + m_pixmap.height() / 2);
  const QLine l1(y1, y2);
  const QLine l2(x1, x2);
  
  painter.setPen("grey");
  painter.drawLine(l1);
  painter.drawLine(l2);
}

void FractalWidget::addControls(QToolBar* toolbar)
{
  toolbar->addWidget(m_fractalEdit);
  toolbar->addWidget(m_firstPassEdit);
  toolbar->addWidget(m_passesEdit);
  toolbar->addSeparator();
  toolbar->addWidget(m_coloursEdit);
  toolbar->addWidget(m_coloursMinWaveEdit);
  toolbar->addWidget(m_coloursMaxWaveEdit);
  toolbar->addSeparator();
  toolbar->addWidget(m_centerEdit);
  toolbar->addWidget(m_scaleEdit);
  toolbar->addWidget(m_divergeEdit);
  toolbar->addSeparator();
  toolbar->addWidget(m_axesEdit);
}

void FractalWidget::addJuliaControls(QToolBar* toolbar)
{
  toolbar->addWidget(m_juliaEdit);
  toolbar->addWidget(m_juliaExponentEdit);
  
  m_juliaToolBar = toolbar;
  m_juliaToolBar->setEnabled(m_fractalName == "julia set");
}

void FractalWidget::init()
{
  m_axesEdit = new QCheckBox("Axes");
  m_axesEdit->setChecked(false);
  m_axesEdit->setToolTip("toggle axes");
  
  m_centerEdit = new QLineEdit();
  m_centerEdit->setText(
    QString::number(m_center.x()) + "," + QString::number(m_center.y()));
  m_centerEdit->setToolTip("center x,y");
  
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
  
  m_divergeEdit = new QLineEdit();
  m_divergeEdit->setText(QString::number(m_diverge));
  m_divergeEdit->setValidator(new QDoubleValidator());
  m_divergeEdit->setFixedWidth(25);
  m_divergeEdit->setToolTip("diverge");

  m_firstPassEdit = new QSpinBox();
  m_firstPassEdit->setMaximum(32);
  m_firstPassEdit->setMinimum(1);
  m_firstPassEdit->setValue(m_firstPass);
  m_firstPassEdit->setToolTip("first pass");

  m_fractalEdit = new QComboBox();
  
  for (uint i = 0; i < Fractal::names().size(); i++)
  {
    m_fractalEdit->addItem(
      QString::fromStdString(Fractal::names()[i]));
  }  
  
  const int index = m_fractalEdit->findText(m_fractalName);
  
  if (index != -1)
  {
    m_fractalEdit->setCurrentIndex(index);
  }
  
  m_fractalEdit->setToolTip("fractal to observe");
  
  m_juliaEdit = new QLineEdit();
  m_juliaEdit->setText(
    QString::number(m_julia.real()) + "," + QString::number(m_julia.imag()));
  m_juliaEdit->setToolTip("julia real,imag");
  
  m_juliaExponentEdit = new QLineEdit();
  m_juliaExponentEdit->setText(QString::number(m_juliaExponent));
  m_juliaExponentEdit->setValidator(new QDoubleValidator());
  m_juliaExponentEdit->setToolTip("julia exponent");

  m_passesEdit = new QSpinBox();
  m_passesEdit->setMaximum(32);
  m_passesEdit->setMinimum(m_firstPassEdit->value());
  m_passesEdit->setValue(m_passes);
  m_passesEdit->setToolTip("last pass");
  
  m_scaleEdit = new QLineEdit();
  m_scaleEdit->setText(QString::number(m_scale));
  m_scaleEdit->setValidator(new QDoubleValidator());
  m_scaleEdit->setToolTip("scale");

  m_passesLabel = new QLabel();
  m_passesLabel->setToolTip("current pass out of max passes");
  m_updatesLabel = new QLabel();
  m_updatesLabel->setToolTip("total images rendered");
  
  connect(&m_thread, SIGNAL(renderedImage(QImage,uint,uint,double,bool)),
    this, SLOT(updatePixmap(QImage,uint,uint,double,bool)));
  connect(&m_thread, SIGNAL(renderingImage(uint,uint,uint)),
    this, SLOT(updatePass(uint,uint,uint)));
    
  connect(m_axesEdit, SIGNAL(stateChaged(bool)),
    this, SLOT(setAxes(bool)));
  connect(m_centerEdit, SIGNAL(textEdited(const QString&)),
    this, SLOT(setCenter(const QString&)));
  connect(m_colourDialog, SIGNAL(colorSelected(const QColor&)),
    this, SLOT(setColourSelected(const QColor&)));
  connect(m_coloursEdit, SIGNAL(valueChanged(int)),
    this, SLOT(setColoursMax(int)));
  connect(m_coloursMinWaveEdit, SIGNAL(valueChanged(int)),
    this, SLOT(setColoursMinWave(int)));
  connect(m_coloursMaxWaveEdit, SIGNAL(valueChanged(int)),
    this, SLOT(setColoursMaxWave(int)));
  connect(m_divergeEdit, SIGNAL(textEdited(const QString&)),
    this, SLOT(setDiverge(const QString&)));
  connect(m_firstPassEdit, SIGNAL(valueChanged(int)),
    this, SLOT(setFirstPass(int)));
  connect(m_fractalEdit, SIGNAL(currentIndexChanged(const QString&)),
    this, SLOT(setFractal(const QString&)));
  connect(m_juliaEdit, SIGNAL(textEdited(const QString&)),
    this, SLOT(setJulia(const QString&)));
  connect(m_juliaExponentEdit, SIGNAL(textEdited(const QString&)),
    this, SLOT(setJuliaExponent(const QString&)));
  connect(m_passesEdit, SIGNAL(valueChanged(int)),
    this, SLOT(setPasses(int)));
  connect(m_scaleEdit, SIGNAL(textEdited(const QString&)),
    this, SLOT(setScale(const QString&)));

  setCursor(Qt::CrossCursor);
  setFocusPolicy(Qt::StrongFocus);
  
  m_statusBar->addPermanentWidget(m_passesLabel);
  m_statusBar->addPermanentWidget(m_updatesLabel);
}

void FractalWidget::copy()
{
  QApplication::clipboard()->setImage(m_pixmap.toImage());
}

void FractalWidget::keyPressEvent(QKeyEvent *event)
{
  const int scrollStep = 20;
  
  switch (event->key()) 
  {
    case Qt::Key_Plus:
      zoom(zoomIn);
      break;
    case Qt::Key_Minus:
      zoom(1 / zoomIn);
      break;
    case Qt::Key_Left:
      scroll(QPoint(-scrollStep, 0));
      break;
    case Qt::Key_Right:
      scroll(QPoint(+scrollStep, 0));
      break;
    case Qt::Key_Down:
      scroll(QPoint(0, +scrollStep));
      break;
    case Qt::Key_Up:
      scroll(QPoint(0, -scrollStep));
      break;
    default:
      QWidget::keyPressEvent(event);
  }
}

void FractalWidget::mouseMoveEvent(QMouseEvent *event)
{
  if (event->buttons() & Qt::LeftButton) 
  {
    m_pixmapOffset += event->pos() - m_lastDragPos;
    m_lastDragPos = event->pos();
    update();
  }
}

void FractalWidget::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
    m_lastDragPos = event->pos();
}

void FractalWidget::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
  {
    m_pixmapOffset += event->pos() - m_lastDragPos;
    m_lastDragPos = QPoint();

    QPoint delta(
     (width() - m_pixmap.width()) / 2 + m_pixmapOffset.x(),
     (height() - m_pixmap.height()) / 2 + m_pixmapOffset.y());
     
    scroll(delta);
  }
}

void FractalWidget::paintEvent(QPaintEvent * /* event */)
{
  if (m_pixmap.isNull()) 
  {
    return;
  }

  QPainter painter(this);

  if (m_scale == m_pixmapScale) 
  {
    painter.drawPixmap(m_pixmapOffset, m_pixmap);
    addAxes(painter);
  } 
  else 
  {
    const double scaleFactor = m_pixmapScale / m_scale;
         
    painter.save();
    painter.translate(m_pixmapOffset);
    painter.scale(scaleFactor, scaleFactor);
    painter.translate(-m_pixmapOffset);
    painter.restore();
    QRectF exposed = painter.matrix().inverted().mapRect(rect()).adjusted(-1, -1, 1, 1);
    painter.drawPixmap(exposed, m_pixmap, exposed);
    addAxes(painter);
  }
}

void FractalWidget::render(int start_at)
{
  update();
  
  Fractal fractal(&m_thread, 
    m_fractalName.toStdString(), m_diverge, m_julia, m_juliaExponent);
  
  if (fractal.isOk())
  {
    m_thread.render(
      fractal,
      QImage(size(), QImage::Format_RGB32), 
      m_center, 
      m_scale, 
      (start_at > 0 ? start_at: m_firstPass), 
      m_passes, 
      m_colours);
  }
}

void FractalWidget::resizeEvent(QResizeEvent * /* event */)
{
  render();
}

void FractalWidget::save()
{
  QSettings settings;
  
  settings.setValue("center", m_center);
  settings.setValue("colours", m_colours.size());
  settings.setValue("first pass", m_firstPass);
  settings.setValue("fractal", m_fractalName);
  settings.setValue("julia exponent", m_juliaExponent);
  settings.setValue("julia real", m_julia.real());
  settings.setValue("julia imag", m_julia.imag());
  settings.setValue("last pass", m_passes);
  settings.setValue("scale", m_scale);
}

void FractalWidget::scroll(const QPoint& delta)
{
  m_center -= QPointF(delta) * m_scale;
  m_origin += delta;
  m_centerEdit->setText(
    QString::number(m_center.x()) + "," + QString::number(m_center.y()));
  
  render();
}

void FractalWidget::setAxes(bool /* state */)
{
  render();
}

void FractalWidget::setCenter(const QString& text)
{
  const QStringList sl(text.split(","));
  
  if (sl.size() == 2)
  {
    m_center = QPointF(
      sl[0].toDouble(),
      sl[1].toDouble());
      
    render();
  }
}

void FractalWidget::setColourSelected(const QColor& color)
{
  if (!color.isValid())
  {
    if (m_colourIndex > 0)
    {
      render();
    }
    
    return;
  }
  
  m_colours[m_colourIndex] = color.rgb();
  
  if (m_colourIndexFromStart)
  {
    if (m_colourIndex < m_colours.size() - 1)
      m_colourIndex++;
    else
      return;
  }
  else
  {
    if (m_colourIndex >= 1)
      m_colourIndex--;
    else
      return;
  }
    
  m_colourDialog->setCurrentColor(m_colours[m_colourIndex]);
  m_colourDialog->setWindowTitle(
    QString("Select Colour %1 of %2")
      .arg(m_colourIndex + 1).arg(m_colours.size()));
  m_colourDialog->show();
}

void FractalWidget::setColours(uint colours)
{
  m_colours.clear();
  
  const double visible_min = m_coloursMinWave;
  const double visible_max = m_coloursMaxWave;

  for (uint i = 0; i < colours - 1; ++i)
  {
    m_colours.push_back(
      wav2RGB(visible_min + (i * (visible_max - visible_min) / colours)));
  }
  
  m_colours.push_back(qRgb(0, 0, 0));
}

void FractalWidget::setColoursDialog(bool from_start)
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
  
void FractalWidget::setColoursMax(int value)
{
  if (value > 0)
  {
    setColours(value);
    render(m_pass);
  }
}

void FractalWidget::setColoursMaxWave(int value)
{
  if (value > 0)
  {
    m_coloursMaxWave = value;
    setColours(m_colours.size());
    render(m_pass);
  }
}

void FractalWidget::setColoursMinWave(int value)
{
  if (value > 0)
  {
    m_coloursMinWave = value;
    setColours(m_colours.size());
    render(m_pass);
  }
}

void FractalWidget::setDiverge(const QString& text)
{
  if (text.isEmpty())
  {
    return;
  }
  
  m_diverge = text.toDouble();
  render();
}

void FractalWidget::setFirstPass(int value)
{
  if (value > 0)
  {
    m_passesEdit->setMinimum(value);
    m_firstPass = value;
    render();
  }
}

void FractalWidget::setFractal(const QString& index)
{
  if (!index.isEmpty())
  {
    m_fractalName = index;
    
    if (m_juliaToolBar != NULL)
    {
      m_juliaToolBar->setEnabled(m_fractalName == "julia set");
    }
    
    m_pass = 0;
    render();
  }
}

void FractalWidget::setJulia(const QString& text)
{
  const QStringList sl(text.split(","));
  
  if (sl.size() != 2)
  {
    return;
  }
      
  m_julia = std::complex<double>(sl[0].toDouble(), sl[1].toDouble());
  
  render();
}      

void FractalWidget::setJuliaExponent(const QString& text)
{
  if (text.isEmpty())
  {
    return;
  }
  
  m_juliaExponent = text.toDouble();
  
  render();
}

void FractalWidget::setPasses(int value)
{
  if (value > 0)
  {
    m_passes = value;
    render(m_pass + 1);
  }
}

void FractalWidget::setScale(const QString& text)
{
  if (text.isEmpty())
  {
    return;
  }
  
  const double scale = text.toDouble();
  
  if (scale != 0)
  {
    m_scale = scale;
    render();
  }
}

void FractalWidget::updatePass(uint pass, uint max, uint iterations)
{
  m_pass = pass;
  
  m_passesLabel->setText(QString::number(pass) + "," + QString::number(max));
  
  m_statusBar->showMessage(QString("executing %1 iterations ...")
    .arg(iterations));
}

void FractalWidget::updatePixmap(
  const QImage &image, uint pass, uint max, double scale, bool snapshot)
{
  m_updates++;
  m_updatesLabel->setText(QString::number(m_updates));
    
  if (!snapshot)
  {
    m_passesLabel->setText(QString::number(pass) + "," + QString::number(max));
     
    if (pass == max)
    {
      m_statusBar->showMessage("ready");  
    }
  }
  else
  {
    m_statusBar->showMessage("refreshed", 50);
  }
  
  if (!m_thread.isRunning())
  {
    m_statusBar->showMessage("stopped");
  }
  
  if (!m_lastDragPos.isNull())
    return;
    
  m_pixmap = QPixmap::fromImage(image);
  m_pixmapOffset = QPoint();
  m_lastDragPos = QPoint();
  m_pixmapScale = scale;
  
  update();
}

// see
// http://codingmess.blogspot.com/2009/05/conversion-of-wavelength-in-nanometers.html
uint FractalWidget::wav2RGB(double w) const
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

void FractalWidget::wheelEvent(QWheelEvent *event)
{
  const int numDegrees = event->delta() / 8;
  const double numSteps = numDegrees / 15.0f;
  zoom(pow(zoomIn, numSteps));
}

void FractalWidget::zoom(double zoomFactor)
{
  m_scale *= zoomFactor;
  m_center *= zoomFactor;
  m_scaleEdit->setText(QString::number(m_scale));
  
  render();
}
