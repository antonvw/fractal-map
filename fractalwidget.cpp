////////////////////////////////////////////////////////////////////////////////
// Name:      fractalwidget.cpp
// Purpose:   Implementation of class FractalWidget
// Author:    Anton van Wezenbeek
// Copyright: (c) 2014 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <QApplication>
#include <QtGui>
#include <QRegExpValidator>
#include <qwt_plot_grid.h>
#include "fractalwidget.h"
#include "fractal.h"
#include "plotitem.h"
#include "plotzoomer.h"

FractalWidget::FractalWidget(
  QWidget* parent,
  QStatusBar* statusbar,
  const QString& fractalName,
  int colours,
  const QString& dir,
  double diverge,
  const QwtInterval& xInterval,
  const QwtInterval& yInterval,
  int first_pass,
  int passes,
  double julia_real,
  double julia_imag,
  double julia_exponent,
  bool show_axes)
  : QwtPlot(parent)
  , Fractal(fractalName.toStdString(), 
      diverge,
      std::complex<double>(julia_real, julia_imag),
      julia_exponent)
  , m_fractalGeo(xInterval, yInterval,
      first_pass,
      passes,
      colours,
      dir)
  , m_fractalPixmap(100, 100)
  , m_autoZoom(-1)
  , m_updates(0)
  , m_juliaToolBar(NULL)
  , m_progressBar(new QProgressBar())
  , m_statusBar(statusbar)
  , m_toolBar(NULL)
{
  init(show_axes);
}

FractalWidget::FractalWidget(
  const FractalWidget& fw, QStatusBar* statusbar)
  : QwtPlot(fw.parentWidget())
  , Fractal(fw)
  , m_fractalGeo(fw.m_fractalGeo)
  , m_autoZoom(fw.m_autoZoom)
  , m_updates(0)
  , m_juliaToolBar(fw.m_juliaToolBar)
  , m_progressBar(new QProgressBar())
  , m_statusBar(statusbar)
  , m_toolBar(fw.m_toolBar)
{
  init(fw.m_axesEdit->isChecked());
  
  if (!fw.m_fractalPixmap.isNull())
  { 
    m_fractalPixmap = fw.m_fractalPixmap;
    update();
  }
}

void FractalWidget::addControls(QToolBar* toolbar)
{
  toolbar->addWidget(m_fractalEdit);
  toolbar->addWidget(m_sizeEdit);
  toolbar->addSeparator();
  
  toolbar->addWidget(m_divergeEdit);
  toolbar->addSeparator();
  toolbar->addWidget(m_axesEdit);
  
  m_toolBar = toolbar;
}

void FractalWidget::addGeometryControls(QToolBar* toolbar)
{
  m_fractalGeo.addControls(toolbar);
}

void FractalWidget::addJuliaControls(QToolBar* toolbar)
{
  toolbar->addWidget(m_juliaEdit);
  toolbar->addWidget(m_juliaExponentEdit);
  
  m_juliaToolBar = toolbar;
  m_juliaToolBar->setVisible(name() == "julia set");
}

void FractalWidget::autoZoom()
{
  m_autoZoom = 0;
  
  zoom();
}

void FractalWidget::copy()
{
  QApplication::clipboard()->setImage(m_fractalPixmap.toImage());
  m_statusBar->showMessage("copied to clipboard", 50);
}

bool FractalWidget::doubleClicked()
{
  if (!m_fractalGeo.useImages())
  {
    QImage image(m_fractalPixmap.toImage());
    QRgb rgb = image.pixel(m_zoom->trackerPosition());
    
    const QColor color = QColorDialog::getColor(QColor(rgb));
    
    if (color.isValid())
    {
      m_fractalGeo.setColours(rgb, color.rgb());
      return true;
    }
  }

  return false;
}

void FractalWidget::init(bool show_axes)
{
  // Qwt uses a minimumSizeHit, override that.
  setMinimumSize(32, 32);
  
  setAxisScale(xBottom, 
    m_fractalGeo.intervalX().minValue(), 
    m_fractalGeo.intervalX().maxValue());
    
  setAxisScale(yLeft, 
    m_fractalGeo.intervalY().minValue(), 
    m_fractalGeo.intervalY().maxValue());

  m_axesEdit = new QCheckBox("Axes");
  m_axesEdit->setToolTip("toggle axes");
  m_axesEdit->setChecked(show_axes);
  
  m_divergeEdit = new QLineEdit();
  m_divergeEdit->setText(QString::number(diverge()));
  m_divergeEdit->setValidator(new QDoubleValidator());
  m_divergeEdit->setFixedWidth(25);
  m_divergeEdit->setToolTip("diverge");

  m_fractalEdit = new QComboBox();
  
  for (uint i = 0; i < Fractal::names().size(); i++)
  {
    m_fractalEdit->addItem(
      QString::fromStdString(Fractal::names()[i]));
  }  
  
  const int index = m_fractalEdit->findText(QString::fromStdString(name()));
  
  if (index != -1)
  {
    m_fractalEdit->setCurrentIndex(index);
  }
  
  m_fractalEdit->setToolTip("fractal to observe");
  
  m_juliaEdit = new QLineEdit();
  m_juliaEdit->setText(
    QString::number(julia().real()) + "," + QString::number(julia().imag()));
  m_juliaEdit->setToolTip("julia real,imag");
  m_juliaEdit->setValidator(new QRegExpValidator(QRegExp(pointf_regexp)));
  
  m_juliaExponentEdit = new QLineEdit();
  m_juliaExponentEdit->setText(QString::number(juliaExponent()));
  m_juliaExponentEdit->setValidator(new QDoubleValidator());
  m_juliaExponentEdit->setToolTip("julia exponent");

  m_sizeEdit = new QLineEdit();
  m_sizeEdit->setToolTip("fractal size");
  m_sizeEdit->setValidator(new QRegExpValidator(QRegExp(size_regexp)));
  
  m_passesLabel = new QLabel();
  m_passesLabel->setToolTip("current pass out of max passes");
  m_updatesLabel = new QLabel();
  m_updatesLabel->setToolTip("total images rendered");
  
  connect(&m_fractalGeo, SIGNAL(changed()),
    this, SLOT(render()));
    
  connect(&m_fractalRenderer, SIGNAL(rendered(QImage,int)),
    this, SLOT(updatePixmap(QImage,int)));
  connect(&m_fractalRenderer, SIGNAL(rendering(int,int,int)),
    this, SLOT(updatePass(int,int,int)));
  connect(&m_fractalRenderer, SIGNAL(rendering(int,int)),
    this, SLOT(updatePass(int,int)));
    
  connect(m_axesEdit, SIGNAL(stateChanged(int)),
    this, SLOT(setAxes(int)));
  connect(m_divergeEdit, SIGNAL(textEdited(const QString&)),
    this, SLOT(setDiverge(const QString&)));
  connect(m_fractalEdit, SIGNAL(currentIndexChanged(const QString&)),
    this, SLOT(setFractal(const QString&)));
  connect(m_juliaEdit, SIGNAL(returnPressed()),
    this, SLOT(setJulia()));
  connect(m_juliaExponentEdit, SIGNAL(textEdited(const QString&)),
    this, SLOT(setJuliaExponent(const QString&)));
  connect(m_sizeEdit, SIGNAL(returnPressed()),
    this, SLOT(setSize()));

  m_statusBar->addPermanentWidget(m_progressBar);
  m_statusBar->addPermanentWidget(m_passesLabel);
  m_statusBar->addPermanentWidget(m_updatesLabel);
  m_progressBar->hide();
  
  m_grid = new QwtPlotGrid;
  m_grid->enableXMin(true);
  m_grid->enableYMin(true);
  m_grid->setMajorPen(QPen(Qt::white, 0, Qt::DotLine));
  m_grid->setMinorPen(QPen(Qt::darkGray, 0, Qt::DotLine));
  m_grid->setZ(1000); // always on top (last item)
  m_grid->attach(this);
  
  FractalPlotItem* fractalitem = new FractalPlotItem();
  fractalitem->attach(this);
  
  m_zoom = new PlotZoomer(canvas(), m_statusBar);
  
  connect(&m_fractalGeo, SIGNAL(changedIntervals()),
    this, SLOT(setIntervals()));
  connect(m_zoom, SIGNAL(zoomed(const QRectF&)),
    this, SLOT(zoomed()));

  // After grid has been constructed.
  setAxes(show_axes ? Qt::Checked: Qt::Unchecked);
    
  replot();
}

void FractalWidget::render()
{
  m_fractalGeo.setIntervals(
    axisInterval(xBottom), axisInterval(yLeft));

  if (m_autoZoom >= 0)
  {
    // Cannot in autoZoom, as geo setIntervals
    // resets single pass.
    m_fractalGeo.setSinglePass();
  }
  
  if (m_fractalRenderer.render(*this, 
    QImage(size(), QImage::Format_RGB32), 
    m_fractalGeo))
  {
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(size().height());
    m_progressBar->show();
  }
  else if (m_fractalRenderer.allowRender())
  {
    // we could not render, but it is allowed, this is an error
    m_statusBar->showMessage("rendering failed");
  }
}

void FractalWidget::resizeEvent(QResizeEvent* event)
{
  QwtPlot::resizeEvent(event);
  
  render();
  replot();
  
  m_sizeEdit->setText(
    QString::number(size().width()) + "," + QString::number(size().height()));
}

void FractalWidget::save()
{
  QSettings settings;
  
  settings.setValue("axes", m_axesEdit->isChecked());
  settings.setValue("colours", (int)m_fractalGeo.colours().size());
  settings.setValue("first pass", m_fractalGeo.firstPass());
  settings.setValue("fractal", QString::fromStdString(name()));
  settings.setValue("julia exponent", juliaExponent());
  settings.setValue("julia real", julia().real());
  settings.setValue("julia imag", julia().imag());
  settings.setValue("last pass", m_fractalGeo.maxPasses());
  settings.setValue("diverge", diverge());
  settings.setValue("dir", m_fractalGeo.dir().absolutePath());
}

void FractalWidget::setAxes(int state)
{
  const bool use = (state == Qt::Checked);
  
  enableAxis(xBottom, use);
  enableAxis(yLeft, use);
  
  use ? m_grid->show(): m_grid->hide();
}

void FractalWidget::setDiverge(const QString& text)
{
  if (text.isEmpty())
  {
    return;
  }
  
  Fractal::setDiverge(text.toDouble());
  
  render();
}

void FractalWidget::setFractal(const QString& index)
{
  if (!index.isEmpty())
  {
    setName(index.toStdString());
    
    if (m_juliaToolBar != NULL)
    {
      m_juliaToolBar->setVisible(name() == "julia set");
    }
    
    render();
  }
}

void FractalWidget::setIntervals()
{
  setAxisScale(xBottom, 
    m_fractalGeo.intervalX().minValue(), 
    m_fractalGeo.intervalX().maxValue());
    
  setAxisScale(yLeft, 
    m_fractalGeo.intervalY().minValue(), 
    m_fractalGeo.intervalY().maxValue());
    
  m_zoom->setZoomBase();
    
  render();
}
    
void FractalWidget::setJulia()
{
  const QStringList sl(m_juliaEdit->text().split(","));
  
  if (sl.size() != 2)
  {
    return;
  }
      
  Fractal::setJulia(std::complex<double>(sl[0].toDouble(), sl[1].toDouble()));
  
  render();
}      

void FractalWidget::setJuliaExponent(const QString& text)
{
  if (text.isEmpty())
  {
    return;
  }
  
  Fractal::setJuliaExponent(text.toDouble());
  
  render();
}

void FractalWidget::setSize()
{
  const QStringList sl(m_sizeEdit->text().split(","));
  
  int height = 0;
  int width = 0;
  
  if (m_toolBar != NULL)
  {
    if (
      m_toolBar->isVisible() && 
     !m_toolBar->isFloating())
    {
      if (m_toolBar->orientation() == Qt::Horizontal)
      {
        height += m_toolBar->height();
      }
      else
      {
        width += m_toolBar->width();
      }
    }
  }
  
  if (m_juliaToolBar != NULL)
  {
    if (
      m_juliaToolBar->isVisible() && 
     !m_juliaToolBar->isFloating())
    {
      if (m_juliaToolBar->orientation() == Qt::Horizontal)
      {
        height += m_juliaToolBar->height();
      }
      else
      {
        width += m_toolBar->width();
      }
    }
  }
  
  if (sl.size() == 2)
  {
    parentWidget()->resize(QSize(
      sl[0].toInt() + width, 
      sl[1].toInt() + m_statusBar->height() + height));
  }
}

void FractalWidget::updatePass(int line, int /*max */)
{
  m_progressBar->setValue(line);
}

void FractalWidget::updatePass(int pass, int max, int iterations)
{
  if (pass > 0 && max > 0)
  {
    m_passesLabel->show();
    m_passesLabel->setText(QString::number(pass) + "," + QString::number(max));
  }
  else
  {
    m_passesLabel->hide();
  }
    
  m_statusBar->showMessage(QString("executing %1 iterations")
    .arg(iterations));
  m_time.start();    
}

void FractalWidget::updatePixmap(const QImage &image, int state)
{
  m_updates++;
  m_updatesLabel->setText(QString::number(m_updates));
    
  switch (state)
  {
  case RENDERING_ACTIVE:
    break;
  
  case RENDERING_READY:
    m_progressBar->hide();
    m_statusBar->showMessage("ready");
    m_statusBar->setToolTip(QString::number((double)m_time.elapsed() / 1000 ));
    
    if (m_autoZoom >= 0)
    {
      zoom();
      m_autoZoom++;
      
      if (m_autoZoom >= 75)
      {
        m_autoZoom = -1;
      }
    }
    break;
    
  case RENDERING_SNAPSHOT:
    m_statusBar->showMessage("refreshed", 50);
    break;
  }
    
  m_fractalPixmap = QPixmap::fromImage(image);
  
  replot();
}

void FractalWidget::zoom()
{
  const double minfactor = 0.9;
  
  const QPointF center = m_zoom->zoomRect().center();
  const double width = m_zoom->zoomRect().width() * minfactor;
  const double height = m_zoom->zoomRect().height() * minfactor;
  
  const QRectF r(
    center.x() - width / 2.0, 
    center.y() - height / 2.0,
    width, 
    height); 
    
  m_zoom->zoom(r);
}

void FractalWidget::zoomed()
{
  // Just render, might restore pixmaps from cache if we are zooming back
  // or forward to recently rendered fractal,
  // so rendering would not be necessary.
  render();  
}
