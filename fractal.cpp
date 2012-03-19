#include <math.h>
#include <QtGui>
#include "fractal.h"

const double zoomIn = 0.8;

Fractal::Fractal(
  QMainWindow* mainwindow,
  QWidget* parent,
  double scale,
  uint colours,
  double diverge,
  const QPointF& center,
  uint first_pass,
  uint passes,
  const QImage& image,
  int fractal_type)
  : QWidget(parent)
  , m_centerEdit(new QLineEdit())
  , m_coloursEdit(new QSpinBox())
  , m_divergeEdit(new QLineEdit())
  , m_first_passEdit(new QSpinBox())
  , m_fractalEdit(new QComboBox())
  , m_passesEdit(new QSpinBox())
  , m_scaleEdit(new QLineEdit())
  , m_origin(0, 0)
  , m_center(center)
  , m_fractalType(fractal_type)
  , m_diverge(diverge)
  , m_pixmapScale(scale)
  , m_scale(scale)
  , m_first_pass(first_pass)
  , m_pass(first_pass)
  , m_passes(passes)
  , m_updates(0)
  , m_mainWindow(mainwindow)
{
  m_centerEdit->setText(
    QString::number(m_center.x()) + "," + QString::number(m_center.y()));
  m_centerEdit->setToolTip("center x,y");
  
  m_coloursEdit->setMaximum(8192);
  m_coloursEdit->setValue(colours);
  m_coloursEdit->setToolTip("colours");
  
  m_divergeEdit->setText(QString::number(m_diverge));
  m_divergeEdit->setToolTip("diverge");

  m_first_passEdit->setMaximum(32);
  m_first_passEdit->setMinimum(1);
  m_first_passEdit->setValue(m_first_pass);
  m_first_passEdit->setToolTip("first pass");
  
  m_fractalEdit->addItem("mandelbrot set");
  m_fractalEdit->addItem("julia set");
  m_fractalEdit->addItem("julia set golden");
  m_fractalEdit->addItem("julia set dragon");
  m_fractalEdit->addItem("julia set mis");
  m_fractalEdit->setToolTip("fractal to observe");
  
  m_passesEdit->setMaximum(32);
  m_passesEdit->setMinimum(m_first_passEdit->value());
  m_passesEdit->setValue(m_passes);
  m_passesEdit->setToolTip("passes");
  
  m_scaleEdit->setText(QString::number(m_scale));
  m_scaleEdit->setToolTip("scale");

  connect(&m_thread, SIGNAL(renderedImage(QImage,double)),
    this, SLOT(updatePixmap(QImage,double)));
  connect(&m_thread, SIGNAL(renderingImage(uint,uint,uint)),
    this, SLOT(updatePass(uint,uint,uint)));
    
  connect(m_centerEdit, SIGNAL(textEdited(const QString&)),
    this, SLOT(editedCenter(const QString&)));
  connect(m_coloursEdit, SIGNAL(valueChanged(int)),
    this, SLOT(editedColours(int)));
  connect(m_divergeEdit, SIGNAL(textEdited(const QString&)),
    this, SLOT(editedDiverge(const QString&)));
  connect(m_first_passEdit, SIGNAL(valueChanged(int)),
    this, SLOT(editedFirstPass(int)));
  connect(m_fractalEdit, SIGNAL(currentIndexChanged(int)),
    this, SLOT(editedFractal(int)));
  connect(m_passesEdit, SIGNAL(valueChanged(int)),
    this, SLOT(editedPasses(int)));
  connect(m_scaleEdit, SIGNAL(textEdited(const QString&)),
    this, SLOT(editedScale(const QString&)));

  setColours(colours);
  setCursor(Qt::CrossCursor);
  setFocusPolicy(Qt::StrongFocus);
 
  if (!image.isNull())
  { 
    m_pixmap = QPixmap::fromImage(image);
    update();
  }
}

void Fractal::editedCenter(const QString& text)
{
  QStringList sl(text.split(","));
  
  if (sl.size() == 2)
  {
    m_center = QPointF(
      sl[0].toDouble(),
      sl[1].toDouble());
      
    render();
  }
}

void Fractal::editedColours(int value)
{
  if (value > 0)
  {
    setColours(value);
    render(m_pass);
  }
}

void Fractal::editedDiverge(const QString& text)
{
  if (text.isEmpty())
  {
    return;
  }
  
  m_diverge = text.toDouble();
  render();
}

void Fractal::editedFirstPass(int value)
{
  if (value > 0)
  {
    m_passesEdit->setMinimum(value);
    
    m_first_pass = value;
    render();
  }
}

void Fractal::editedFractal(int index)
{
  if (index >= 0)
  {
    m_fractalType = index;
    m_pass = 0;
    
    render();
  }
}

void Fractal::editedPasses(int value)
{
  if (value > 0)
  {
    m_passes = value;
    render(m_pass);
  }
}

void Fractal::editedScale(const QString& text)
{
  if (text.isEmpty())
  {
    return;
  }
  
  m_scale = text.toDouble();
  render(m_pass);
}

void Fractal::keyPressEvent(QKeyEvent *event)
{
  m_mainWindow->statusBar()->showMessage(QString("key: %1").arg((int)event->key()));
  
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

void Fractal::mouseMoveEvent(QMouseEvent *event)
{
  if (event->buttons() & Qt::LeftButton) 
  {
    m_pixmapOffset += event->pos() - m_lastDragPos;
    m_lastDragPos = event->pos();
    update();
  }
}

void Fractal::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
      m_lastDragPos = event->pos();
}

void Fractal::mouseReleaseEvent(QMouseEvent *event)
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

void Fractal::paintEvent(QPaintEvent * /* event */)
{
  if (m_pixmap.isNull()) 
  {
    return;
  }

  QPainter painter(this);
  painter.fillRect(rect(), Qt::black);

  if (m_scale == m_pixmapScale) 
  {
    painter.drawPixmap(m_pixmapOffset, m_pixmap);
  } 
  else 
  {
    const double scaleFactor = m_pixmapScale / m_scale;
    
    int newWidth = int(m_pixmap.width() * scaleFactor);
    int newHeight = int(m_pixmap.height() * scaleFactor);
    
    QPoint point(
      m_pixmapOffset.x() + (m_pixmap.width() - newWidth) / 2, 
      m_pixmapOffset.y() + (m_pixmap.height() - newHeight) / 2);

    painter.save();
    painter.translate(point);
    painter.scale(scaleFactor, scaleFactor);
    QRectF exposed = painter.matrix().inverted().mapRect(rect()).adjusted(-1, -1, 1, 1);
    painter.drawPixmap(exposed, m_pixmap, exposed);
    painter.restore();
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

void Fractal::render(int start_at)
{
  update();
  
  m_thread.render(
    m_center, 
    m_scale, 
    QImage(size(), QImage::Format_RGB32), 
    (start_at > 0 ? start_at: m_first_pass), 
    m_passes, 
    m_colours, 
    m_diverge,
    m_fractalType);
}

void Fractal::resizeEvent(QResizeEvent * /* event */)
{
  render();
}

uint Fractal::rgbFromWaveLength(double wave)
{
  double r = 0.0;
  double g = 0.0;
  double b = 0.0;

  if (wave >= 380.0 && wave <= 440.0) 
  {
    r = -1.0 * (wave - 440.0) / (440.0 - 380.0);
    b = 1.0;
  } 
  else if (wave >= 440.0 && wave <= 490.0) 
  {
    g = (wave - 440.0) / (490.0 - 440.0);
    b = 1.0;
  } 
  else if (wave >= 490.0 && wave <= 510.0) 
  {
    g = 1.0;
    b = -1.0 * (wave - 510.0) / (510.0 - 490.0);
  } 
  else if (wave >= 510.0 && wave <= 580.0) 
  {
    r = (wave - 510.0) / (580.0 - 510.0);
    g = 1.0;
  } 
  else if (wave >= 580.0 && wave <= 645.0) 
  {
    r = 1.0;
    g = -1.0 * (wave - 645.0) / (645.0 - 580.0);
  } 
  else if (wave >= 645.0 && wave <= 780.0) 
  {
    r = 1.0;
  }

  double s = 1.0;
  
  if (wave > 700.0)
    s = 0.3 + 0.7 * (780.0 - wave) / (780.0 - 700.0);
  else if (wave <  420.0)
    s = 0.3 + 0.7 * (wave - 380.0) / (420.0 - 380.0);

  r = pow(r * s, 0.8);
  g = pow(g * s, 0.8);
  b = pow(b * s, 0.8);
  
  return qRgb(int(r * 255), int(g * 255), int(b * 255));
}

void Fractal::scroll(const QPoint& delta)
{
  m_center -= QPointF(delta) * m_scale;
  m_origin += delta;
  
  m_centerEdit->setText(
    QString::number(m_center.x()) + "," + QString::number(m_center.y()));
  
  render();
}

void Fractal::setColours(uint colours)
{
  m_colours.clear();

  for (uint i = 0; i < colours - 1; ++i)
  {
    m_colours.push_back(rgbFromWaveLength(380.0 + (i * 400.0 / colours)));
  }
  
  m_colours.push_back(qRgb(0, 0, 0));
}

void Fractal::start()
{
  m_thread.start();
}
    
void Fractal::stop()
{
  m_thread.stop();
}
    
void Fractal::updatePass(uint pass, uint maxPasses, uint iterations)
{
  m_pass = pass;
  
  m_mainWindow->statusBar()->showMessage(QString("pass %1 of %2 (%3 iterations) ...")
    .arg(pass).arg(maxPasses).arg(iterations));
}

void Fractal::updatePixmap(const QImage &image, double scale)
{
  if (image.isNull())
  {
    return;
  }
  
  m_updates++;
  
  if (!m_thread.isRunning())
  {
    m_mainWindow->statusBar()->showMessage(QString("stopped afer %1 updates").arg(m_updates));  
  }
  else
  {
    m_mainWindow->statusBar()->showMessage(QString("completed %1 updates").arg(m_updates));  
  }
  
  if (!m_lastDragPos.isNull())
    return;

  m_pixmap = QPixmap::fromImage(image);
  m_pixmapOffset = QPoint();
  m_lastDragPos = QPoint();
  m_pixmapScale = scale;
  
  update();
}

void Fractal::wheelEvent(QWheelEvent *event)
{
  int numDegrees = event->delta() / 8;
  double numSteps = numDegrees / 15.0f;
  zoom(pow(zoomIn, numSteps));
}

void Fractal::zoom(double zoomFactor)
{
  m_scale *= zoomFactor;
  m_center *= zoomFactor;
  m_scaleEdit->setText(QString::number(m_scale));
  
  render();
}
