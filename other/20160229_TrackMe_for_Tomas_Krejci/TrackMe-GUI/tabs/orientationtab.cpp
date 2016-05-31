#include "orientationtab.h"
#include "../utilities.h"

#include <ArrayMatrix.hpp>

#include <QVBoxLayout>
#include <QHBoxLayout>

OrientationTab::OrientationTab(QWidget *parent) :
    TrackMeTab(parent)
{
  auto layout = new QVBoxLayout;
  auto viewLayout = new QHBoxLayout;

  rotatedVector_ = new SensorPlot;
  rotatedVector_->setTitle("Vector <1,1,1> rotating with the device");
  rotatedVector_->setAxisTitle(QwtPlot::xBottom, "Time [s.ms]");

  // Projection of canonical base to the XY plane
  topView_ = new FixedAspectRatioPlot;
  topView_->setIntervalHint(QwtPlot::xBottom, QwtInterval(-1, 1));
  topView_->setIntervalHint(QwtPlot::yLeft, QwtInterval(-1, 1));
  topView_->setCanvasBackground(Qt::white);
  topView_->setTitle("Top view");
  topViewCurveX_ = new QwtPlotCurve;
  topViewCurveX_->setPen(SensorPlot::defaultCurvePen(0));
  topViewCurveX_->setRenderHint(QwtPlotItem::RenderAntialiased);
  topViewCurveX_->attach(topView_);
  topViewCurveY_ = new QwtPlotCurve;
  topViewCurveY_->setPen(SensorPlot::defaultCurvePen(1));
  topViewCurveY_->setRenderHint(QwtPlotItem::RenderAntialiased);
  topViewCurveY_->attach(topView_);
  topViewCurveZ_ = new QwtPlotCurve;
  topViewCurveZ_->setPen(SensorPlot::defaultCurvePen(2));
  topViewCurveZ_->setRenderHint(QwtPlotItem::RenderAntialiased);
  topViewCurveZ_->attach(topView_);

  // Projection of canonical base to the XZ plane
  frontView_ = new FixedAspectRatioPlot;
  frontView_->setIntervalHint(QwtPlot::xBottom, QwtInterval(-1, 1));
  frontView_->setIntervalHint(QwtPlot::yLeft, QwtInterval(-1, 1));
  frontView_->setCanvasBackground(Qt::white);
  frontView_->setTitle("Front view");
  frontViewCurveX_ = new QwtPlotCurve;
  frontViewCurveX_->setPen(SensorPlot::defaultCurvePen(0));
  frontViewCurveX_->setRenderHint(QwtPlotItem::RenderAntialiased);
  frontViewCurveX_->attach(frontView_);
  frontViewCurveZ_ = new QwtPlotCurve;
  frontViewCurveZ_->setPen(SensorPlot::defaultCurvePen(2));
  frontViewCurveZ_->setRenderHint(QwtPlotItem::RenderAntialiased);
  frontViewCurveZ_->attach(frontView_);
  frontViewCurveY_ = new QwtPlotCurve;
  frontViewCurveY_->setPen(SensorPlot::defaultCurvePen(1));
  frontViewCurveY_->setRenderHint(QwtPlotItem::RenderAntialiased);
  frontViewCurveY_->attach(frontView_);

  viewLayout->addWidget(topView_);
  viewLayout->addWidget(frontView_);

  layout->addWidget(rotatedVector_);
  layout->addLayout(viewLayout);

  setLayout(layout);
}

/**
 * @brief Slot for receiving new orientation events.
 */
void OrientationTab::addOrientationEvent(const OrientationEvent &event)
{
  // Using matrix multiplication
  using ArrayMatrix::operator*;

  // Convert a rotation matrix event to an axes event
  TrackMe::AxesEvent<OrientationEvent::ValueT,
                     OrientationEvent::TimestampT> axesEvent;
  axesEvent.type = event.type;
  axesEvent.timestamp = event.timestamp;
  ArrayMatrix::Vector<OrientationEvent::ValueT,3> referenceAxis{{ 1,1,1 }};
  axesEvent.value = event.value * referenceAxis;

  // Now calculate top and front view of unit vectors
  ArrayMatrix::Vector<OrientationEvent::ValueT,3> xUnit{{ 1,0,0 }};
  auto xRotated = event.value * xUnit;
  ArrayMatrix::Vector<OrientationEvent::ValueT,3> yUnit{{ 0,1,0 }};
  auto yRotated = event.value * yUnit;
  ArrayMatrix::Vector<OrientationEvent::ValueT,3> zUnit{{ 0,0,1 }};
  auto zRotated = event.value * zUnit;

  topViewCurveX_->setSamples({{0,0},{xRotated[0], xRotated[1]}});
  topViewCurveY_->setSamples({{0,0},{yRotated[0], yRotated[1]}});
  topViewCurveZ_->setSamples({{0,0},{zRotated[0], zRotated[1]}});

  frontViewCurveX_->setSamples({{0,0},{xRotated[0], xRotated[2]}});
  frontViewCurveY_->setSamples({{0,0},{yRotated[0], yRotated[2]}});
  frontViewCurveZ_->setSamples({{0,0},{zRotated[0], zRotated[2]}});

  // convert to plot event and push to queue
  orientationEvents_.append(PlotEvent::from(axesEvent));

  // Remove old events
  auto timestamp = eventFeeder_->latestTimestamp() - PLOT_INTERVAL;
  erase_old_events(orientationEvents_, timestamp);
}

/**
 * @brief Clear the plots and reinitialize.
 */
void OrientationTab::restart()
{
  orientationEvents_.clear();
  TrackMeTab::restart();
}

/**
 * @brief Refresh the plots.
 */
void OrientationTab::replot()
{
  if (orientationEvents_.empty())
    return;

  auto baseTimestamp   = eventFeeder_->baseTimestamp();
  auto latestTimestamp = eventFeeder_->latestTimestamp();
  auto oldestTimestamp = latestTimestamp - PLOT_INTERVAL;

  rotatedVector_->setBaseTimestamp(baseTimestamp);
  rotatedVector_->setAxisScale(QwtPlot::xBottom,
                                 oldestTimestamp, latestTimestamp);
  rotatedVector_->setSamples(orientationEvents_);

  topView_->replot();
  frontView_->replot();
}

/**
 * @brief Export all plots to a SVG file.
 */
void OrientationTab::exportPlots(const QString &path)
{
  rotatedVector_->exportSvg(path + "/orientation_rotated_vector.svg");
  topView_->exportSvg(path + "/orientation_top_view.svg");
  frontView_->exportSvg(path + "/orientation_front_view.svg");
}
