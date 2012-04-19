////////////////////////////////////////////////////////////////////////////////
// Name:      fractalwidget.cpp
// Purpose:   Implementation of class FractalWidget
// Author:    Anton van Wezenbeek
// Copyright: (c) 2012 Anton van Wezenbeek
////////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <QtGui>
#include <QRegExpValidator>
#include "fractalwidget.h"
#include "fractal.h"

const double zoomIn = 0.9;

FractalWidget::FractalWidget(
  QWidget* parent,
  QStatusBar* statusbar,
  const QString& fractalName,
  double scale,
  int colours,
  const QString& dir,
  double diverge,
  const QPointF& center,
  int first_pass,
  int passes,
  double julia_real,
  double julia_imag,
  double julia_exponent)
  : QWidget(parent)
  , Fractal(fractalName.toStdString(), 
      diverge,
      std::complex<double>(julia_real, julia_imag),
      julia_exponent)
  , m_fractalGeo(center,
      scale,
      first_pass,
      passes,
      colours,
      dir)
  , m_fractalPixmapOffset(QPoint())
  , m_fractalPixmapScale(scale)
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
  : QWidget(fw.parentWidget())
  , Fractal(fw)
  , m_fractalGeo(fw.m_fractalGeo)
  , m_fractalPixmapScale(fw.m_fractalPixmapScale)
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

void FractalWidget::addAxes(QPainter& painter)
{
  if (!m_axesEdit->isChecked())
  {
    return;
  }
  
  const QPoint y1 = QPoint(
    m_fractalGeo.origin().x() + m_fractalPixmap.width() / 2, 
    0);
  const QPoint y2 = QPoint(
    m_fractalGeo.origin().x() + m_fractalPixmap.width() / 2, 
    m_fractalPixmap.height());
  const QPoint x1 = QPoint(
    0, 
    m_fractalGeo.origin().y() + m_fractalPixmap.height() / 2);
  const QPoint x2 = QPoint(
    m_fractalPixmap.width(), 
    m_fractalGeo.origin().y() + m_fractalPixmap.height() / 2);
    
  const QLine l1(y1, y2);
  const QLine l2(x1, x2);
  
  painter.setPen("grey");
  painter.drawLine(l1);
  painter.drawLine(l2);
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

  setCursor(Qt::CrossCursor);
  setFocusPolicy(Qt::StrongFocus);
  
  m_statusBar->addPermanentWidget(m_progressBar);
  m_statusBar->addPermanentWidget(m_maxPassesLabel);
  m_statusBar->addPermanentWidget(m_updatesLabel);
}

void FractalWidget::keyPressEvent(QKeyEvent *event)
{
  const int scrollStep = 20;
  
  switch (event->key()) 
  {
    case Qt::Key_Plus:
      m_fractalGeo.zoom(zoomIn);
      break;
    case Qt::Key_Minus:
      m_fractalGeo.zoom(1 / zoomIn);
      break;
    case Qt::Key_Left:
      m_fractalGeo.scroll(QPoint(-scrollStep, 0));
      break;
    case Qt::Key_Right:
      m_fractalGeo.scroll(QPoint(+scrollStep, 0));
      break;
    case Qt::Key_Down:
      m_fractalGeo.scroll(QPoint(0, +scrollStep));
      break;
    case Qt::Key_Up:
      m_fractalGeo.scroll(QPoint(0, -scrollStep));
      break;
    default:
      QWidget::keyPressEvent(event);
  }
}

void FractalWidget::mouseMoveEvent(QMouseEvent *event)
{
  if (event->buttons() & Qt::LeftButton) 
  {
    m_fractalPixmapOffset += event->pos() - m_lastDragPos;
    m_lastDragPos = event->pos();
    update();
  }
}

void FractalWidget::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
  {
    m_lastDragPos = event->pos();
    m_startDragPos = event->pos();
    m_fractalRenderer.interrupt();
  }
}

void FractalWidget::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
  {
    if (m_startDragPos == event->pos())
    {
      if (!m_fractalGeo.useImages())
      {
        QImage image(m_fractalPixmap.toImage());
        QRgb rgb = image.pixel(event->pos());
    
        const QColor color = QColorDialog::getColor(QColor(rgb));
    
        if (color.isValid())
        {
          m_fractalGeo.setColours(rgb, color.rgb());
        }
        else
        {
          if (m_fractalRenderer.interrupted())
          {
            m_fractalRenderer.cont();
          }
        }
      }
      else
      {
        if (m_fractalRenderer.interrupted())
        {
          m_fractalRenderer.cont();
        }
      }
    }
    else
    {
      if (m_fractalRenderer.interrupted())
      {
        m_fractalRenderer.reset();
      }
      
      m_fractalPixmapOffset += event->pos() - m_lastDragPos;
      m_lastDragPos = QPoint();
      m_startDragPos = QPoint();

      QPoint delta(
       (width() - m_fractalPixmap.width()) / 2 + m_fractalPixmapOffset.x(),
       (height() - m_fractalPixmap.height()) / 2 + m_fractalPixmapOffset.y());
     
      m_fractalGeo.scroll(delta);
    }
  }
}

void FractalWidget::paintEvent(QPaintEvent * /* event */)
{
  if (m_fractalPixmap.isNull()) 
  {
    return;
  }

  QPainter painter(this);

  if (m_fractalGeo.scale() == m_fractalPixmapScale) 
  {
    painter.drawPixmap(m_fractalPixmapOffset, m_fractalPixmap);
    addAxes(painter);
  } 
  else 
  {
    const double scaleFactor = m_fractalPixmapScale / m_fractalGeo.scale();
         
    painter.save();
    painter.translate(m_fractalPixmapOffset);
    painter.scale(scaleFactor, scaleFactor);
    painter.translate(-m_fractalPixmapOffset);
    painter.restore();
    QRectF exposed = painter.matrix().inverted().mapRect(rect()).adjusted(-1, -1, 1, 1);
    painter.drawPixmap(exposed, m_fractalPixmap, exposed);
    addAxes(painter);
  }
}

void FractalWidget::render()
{
  update();
  
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

void FractalWidget::resizeEvent(QResizeEvent * /* event */)
{
  render();
  
  m_sizeEdit->setText(
    QString::number(size().width()) + "," + QString::number(size().height()));
}

void FractalWidget::save()
{
  QSettings settings;
  
  settings.setValue("center", m_fractalGeo.center());
  settings.setValue("colours", m_fractalGeo.colours().size());
  settings.setValue("first pass", m_fractalGeo.firstPass());
  settings.setValue("fractal", QString::fromStdString(name()));
  settings.setValue("julia exponent", juliaExponent());
  settings.setValue("julia real", julia().real());
  settings.setValue("julia imag", julia().imag());
  settings.setValue("last pass", m_fractalGeo.maxPasses());
  settings.setValue("scale", m_fractalGeo.scale());
  settings.setValue("diverge", diverge());
  settings.setValue("dir", m_fractalGeo.dir().absolutePath());
}

void FractalWidget::setAxes(int /* state */)
{
  render();
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

void FractalWidget::updatePixmap(
  const QImage &image, double scale, int state)
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
  m_fractalPixmapOffset = QPoint();
  m_fractalPixmapScale = scale;
  
  update();
}

void FractalWidget::wheelEvent(QWheelEvent *event)
{
  const int numDegrees = event->delta() / 8;
  const double numSteps = numDegrees / 15.0f;
  m_fractalGeo.zoom(pow(zoomIn, numSteps));
}
