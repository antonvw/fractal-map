#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

#include <vector>
#include <QMutex>
#include <QSize>
#include <QImage>
#include <QThread>
#include <QPointF>
#include <QWaitCondition>

class RenderThread : public QThread
{
  Q_OBJECT

public:
  RenderThread(QObject *parent = 0);
 ~RenderThread();
  
  void render(
    const QPointF& center,
    double scale,
    const QSize& size,
    int colormap_size = 1024);

signals:
  void renderedImage(const QImage &image, double scale);
  void renderingImage(int pass, int total, int iterations);

protected:
  void run();

private:
  uint rgbFromWaveLength(double wave);

  QMutex m_mutex;
  QWaitCondition m_condition;
  QPointF m_center;
  double m_scale;
  QSize m_size;
  bool m_restart;
  bool m_abort;
  int m_ColormapSize;
  
  std::vector<uint> m_colormap;
};
#endif
