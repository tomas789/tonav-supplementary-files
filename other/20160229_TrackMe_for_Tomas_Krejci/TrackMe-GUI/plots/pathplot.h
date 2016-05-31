#ifndef POSITIONPLOT_H
#define POSITIONPLOT_H

#include "fixedaspectratioplot.h"
#include <qwt_plot_curve.h>
#include <qwt_plot_spectrocurve.h>
#include <qwt_scale_widget.h>
#include <qwt_color_map.h>
#include <qwt_interval.h>

/**
 * @brief Custom color map for PathPlot.
 */
struct ColorMap: public QwtLinearColorMap
{
    ColorMap() :
      QwtLinearColorMap(Qt::darkCyan, Qt::red)
    {
      addColorStop(0.2, Qt::cyan);
      addColorStop(0.6, Qt::green);
      addColorStop(0.8, Qt::yellow);
    }
};

/**
 * @brief The PathPlot class is a wrapper for the FixedAspectRatioPlot, which
 *        draws a 3D path projected to the 2D space. The distance from the
 *        viewer is denoted by the color of the path.
 */
class PathPlot : public FixedAspectRatioPlot
{
    Q_OBJECT
public:
    explicit PathPlot(QWidget *parent = 0);

    template <typename Container>
    void setSamples(const Container &samples);

protected:
    QwtPlotSpectroCurve *spectro_curve_;
    QwtPlotCurve *background_curve_;
};

/**
 * @brief Plot a container of sensor events.
 */
template <typename Container>
void PathPlot::setSamples(const Container &samples)
{
    QwtInterval zInterval;

    if (!samples.empty())
        zInterval = QwtInterval(samples.first().value[2],
                                samples.first().value[2]);
    else
        zInterval = QwtInterval(0.0, 1.0);

    QVector<double> values[3];
    for (auto &value : values)
        value.reserve(samples.size());

    QVector<QwtPoint3D> points;
    points.reserve(samples.size());

    for (auto &sample : samples) {
        points.push_back(QwtPoint3D(sample.value[0],
                                    sample.value[1],
                                    sample.value[2]));

        for (std::size_t i = 0; i < 2; ++i)
            values[i].push_back(sample.value[i]);

        zInterval |= sample.value[2];
    }

    spectro_curve_->setColorRange(zInterval);
    spectro_curve_->setSamples(points);
    background_curve_->setSamples(values[0], values[1]);

    auto rightAxis = axisWidget(QwtPlot::yRight);
    rightAxis->setColorMap(zInterval, new ColorMap{});
    rightAxis->setColorBarEnabled(true);

    this->setAxisScale(QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue());
    this->replot();
}

#endif // POSITIONPLOT_H
