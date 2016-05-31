#ifndef SENSORPLOT_H
#define SENSORPLOT_H

#include "../config.h"

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_draw.h>

#include <QPen>
#include <QTime>
#include <QVector>

/**
 * @brief The TimeScaleDraw class is a custom time format. It prints
 *        the time relative to the first event.
 */
class TimeScaleDraw: public QwtScaleDraw
{
  public:
    TimeScaleDraw()
    {
      setLabelRotation(-30.0);
      setLabelAlignment(Qt::AlignLeft | Qt::AlignBottom);
    }

    void setBaseTimestamp(const double &baseTimestamp)
    {
      baseTimestamp_ = baseTimestamp;
    }

    void setRatio(const double &ratio)
    {
      ratio_ = ratio;
    }

    virtual QwtText label(double v) const override
    {
      QString str;
      QTime time{0,0,0,0};
      double diff = v - baseTimestamp_;

      if (diff < 0.0) {
        str += "-";
        diff = -diff;
      }

      str += time.addMSecs(diff * ratio_ * 1000.0)
                 .toString("s.zzz");

      return str;
    }

  private:
    double baseTimestamp_{0.0};
    double ratio_{0.0};
};

/**
 * @brief The SensorPlot class is a simple wrapper for the QwtPlot, which
 *        plots container of sensor events in one plot with different colors.
 */
class SensorPlot : public QwtPlot
{
    Q_OBJECT

public:
    explicit SensorPlot(QWidget *parent = 0);

    template <typename Container>
    void setSamples(const Container &samples);
    void setBaseTimestamp(double baseTimestamp);
    static QPen defaultCurvePen(int axis);
    void exportSvg(const QString &path);

protected:
    QwtPlotCurve * curves_[3];
    TimeScaleDraw *timeScaleDraw_;
    static QPen defaultGridPen();

};

/**
 * @brief Set the container to be plotted.
 */
template <typename Container>
void SensorPlot::setSamples(const Container &samples)
{
  auto ratio =
    TrackMe::timestampToSeconds<typename Container::value_type>(1);
  timeScaleDraw_->setRatio(ratio);

  QVector<double> tData;
  tData.reserve(samples.size());
  QVector<double> values[3];
  for (auto &value : values)
    value.reserve(samples.size());

  for (auto &sample : samples) {
    tData.push_back(sample.timestamp);
    for (std::size_t i = 0; i < 3; ++i)
      values[i].push_back(sample.value[i]);
  }

  for (std::size_t i = 0; i < 3; ++i)
    curves_[i]->setSamples(tData, values[i]);

  this->replot();
}

#endif // SENSORPLOT_H
