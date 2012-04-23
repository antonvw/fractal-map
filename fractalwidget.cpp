////////////////////////////////////////////////////////////////////////////////
// Name:      fractalwidget.cpp
// Purpose:   Implementation of class FractalWidget
// Author:    Anton van Wezenbeek
// Copyright: (c) 2012 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <QtGui>
#include <QRegExpValidator>
#include <qwt_plot_grid.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include "fractalwidget.h"
#include "fractal.h"

class FractalPlotItem: public QwtPlotItem
{
public:
  FractalPlotItem();

  virtual int rtti() const;

  virtual void draw(QPainter *p,
    const QwtScaleMap &, const QwtScaleMap &,
    const QRectF &rect) const;
};

FractalPlotItem::FractalPlotItem()
{
  setZ(1);
  setRenderHint(QwtPlotItem::RenderAntialiased, true);
}

int FractalPlotItem::rtti() const
{
  return QwtPlotItem::Rtti_PlotUserItem;
}

void FractalPlotItem::draw(QPainter *p, 
  const QwtScaleMap &, 
  const QwtScaleMap &,
  const QRectF &rect) const
{
  const FractalWidget *plot = (FractalWidget *)plot();
    
  p->save();
  p->drawPixmap(rect, plot->fractalPixmap(), rect);
  p->restore();
}

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
  double julia_exponent)
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
  , m_updates(0)
  , m_juliaToolBar(NULL)
  , m_progressBar(new QProgressBar())
  , m_statusBar(statusbar)
  , m_toolBar(NULL)
{
  init();
}

FractalWidget::FractalWidget(
  const FractalWidget& fw, QStatusBar* statusbar)
  : QwtPlot(fw.parentWidget())
  , Fractal(fw)
  , m_fractalGeo(fw.m_fractalGeo)
  , m_updates(0)
  , m_juliaToolBar(fw.m_juliaToolBar)
  , m_progressBar(new QProgressBar())
  , m_statusBar(statusbar)
  , m_toolBar(fw.m_toolBar)
{
  init();
  
  const QImage image(fw.m_fractalPixmap.toImage());
  
  if (!image.isNull())
  { 
    m_fractalPixmap = QPixmap::fromImage(image);
    update();
  }
}

void FractalWidget::addControls(QToolBar* toolbar)
{
  toolbar->addWidget(m_fractalEdit);
  toolbar->addWidget(m_sizeEdit);
  toolbar->addSeparator();
  
  m_fractalGeo.addControls(toolbar);
  
  toolbar->addWidget(m_divergeEdit);
  toolbar->addSeparator();
  toolbar->addWidget(m_axesEdit);
 
  m_toolBar = toolbar;
}

void FractalWidget::addJuliaControls(QToolBar* toolbar)
{
  toolbar->addWidget(m_juliaEdit);
  toolbar->addWidget(m_juliaExponentEdit);
  
  m_juliaToolBar = toolbar;
  m_juliaToolBar->setEnabled(name() == "julia set");
}

void FractalWidget::copy()
{
  QApplication::clipboard()->setImage(m_fractalPixmap.toImage());
  m_statusBar->showMessage("copied to clipboard", 50);
}

void FractalWidget::init()
{
  m_axesEdit = new QCheckBox("Axes");
  m_axesEdit->setChecked(false);
  m_axesEdit->setToolTip("toggle axes");
  m_axesEdit->setChecked(true);
  
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
  
  m_maxPassesLabel = new QLabel();
  m_maxPassesLabel->setToolTip("current pass out of max passes");
  m_updatesLabel = new QLabel();
  m_updatesLabel->setToolTip("total images rendered");
  
  connect(&m_fractalGeo, SIGNAL(changed()),
    this, SLOT(render()));
    
  connect(&m_fractalRenderer, SIGNAL(rendered(QImage,double,int)),
    this, SLOT(updatePixmap(QImage,double,int)));
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
  m_statusBar->addPermanentWidget(m_maxPassesLabel);
  m_statusBar->addPermanentWidget(m_updatesLabel);
  
  FractalPlotItem* fractalitem = new FractalPlotItem();
  fractalitem->attach(this);
  
  // panning with the left mouse button
  new QwtPlotPanner(canvas());

  // zoom in/out with the wheel
  new QwtPlotMagnifier(canvas());
  
  // grid 
  QwtPlotGrid *grid = new QwtPlotGrid;
  grid->enableXMin(true);
  grid->setMajPen(QPen(Qt::white, 0, Qt::DotLine));
  grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
  grid->setZ(1000); // always on top (last item)
  grid->attach(this);

  setAxisScale(xBottom, m_intervalX.minValue(), m_intervalX.maxValue());
  setAxisScale(yLeft, m_intervalY.minValue(), m_intervalY.maxValue());
}

void FractalWidget::render()
{
  m_fractalGeo.setIntervals(
    axisInterval(xBottom), axisInterval(yLeft));
  
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
  
  replot();
}

void FractalWidget::resizeEvent(QResizeEvent * /* event */)
{
  render();
  
  m_sizeEdit->setText(
    QString::number(size().width()) + "," + QString::number(size().height()));
}

void FractalWidget::save()
{
  QSettings settings;
  
  settings.setValue("colours", m_fractalGeo.colours().size());
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
      m_juliaToolBar->setEnabled(name() == "julia set");
    }
    
    render();
  }
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
  
  if (m_toolBar != NULL)
  {
    if (m_toolBar->isVisible() && !m_toolBar->isFloating())
    {
      height += m_toolBar->height();
    }
  }
  
  if (m_juliaToolBar != NULL)
  {
    if (m_juliaToolBar->isVisible() && !m_juliaToolBar->isFloating())
    {
      height += m_juliaToolBar->height();
    }
  }
  
  if (sl.size() == 2)
  {
    parentWidget()->resize(QSize(
      sl[0].toInt(), 
      sl[1].toInt() + m_statusBar->height() + height));
  }
}

void FractalWidget::updatePass(int line, int /*max */)
{
  m_progressBar->setValue(line);
}

void FractalWidget::updatePass(int pass, int max, int iterations)
{
  m_maxPassesLabel->setText(QString::number(pass) + "," + QString::number(max));
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
    break;
    
  case RENDERING_SNAPSHOT:
    m_statusBar->showMessage("refreshed", 50);
    break;
  }
    
  m_fractalPixmap = QPixmap::fromImage(image);
  
  replot();
}
