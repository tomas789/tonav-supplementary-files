#include "displacementtab.h"
#include "../utilities.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

DisplacementTab::DisplacementTab(QWidget *parent) :
    TrackMeTab(parent)
{
  auto layout = new QVBoxLayout;
  auto pathsLayout = new QHBoxLayout;

  // Displacement as a vector
  displacementPlot_ = new SensorPlot;
  displacementPlot_->setAxisTitle(QwtPlot::yLeft, "Value [m]");
  displacementPlot_->setAxisTitle(QwtPlot::xBottom, "Time [s.ms]");

  // Projection to the XY plane
  topView_ = new PathPlot;
  topView_->setTitle("Top view");
  topView_->setAxisTitle(QwtPlot::yLeft, "Y coordinate [m]");
  topView_->setAxisTitle(QwtPlot::xBottom, "X coordinate [m]");
  topView_->setAxisTitle(QwtPlot::yRight, "Z coordinate [m]");

  // Projection to the XY plane
  frontView_ = new PathPlot;
  frontView_->setTitle("Front view");
  frontView_->setAxisTitle(QwtPlot::yLeft, "Z coordinate [m]");
  frontView_->setAxisTitle(QwtPlot::xBottom, "X coordinate [m]");
  frontView_->setAxisTitle(QwtPlot::yRight, "Y coordinate [m]");

  pathsLayout->addWidget(topView_);
  pathsLayout->addWidget(frontView_);
  layout->addWidget(displacementPlot_);
  layout->addLayout(pathsLayout);

  setLayout(layout);
}

/**
 * @brief Slot for new displacement sensor events.
 */
void DisplacementTab::addDisplacementEvent(const RawEvent &event)
{
  // Convert event types
  auto plotEvent = PlotEvent::from(event);

  topViewEvents_.append(plotEvent);
  // Swap Y and Z axis for a front view
  std::swap(plotEvent.value[1], plotEvent.value[2]);
  frontViewEvents_.append(plotEvent);

  // Remove old events
  auto timestamp = eventFeeder_->latestTimestamp() - PLOT_INTERVAL;
  erase_old_events(topViewEvents_, timestamp);
  erase_old_events(frontViewEvents_, timestamp);
}

/**
 * @brief Clear the plots and reinitialize.
 */
void DisplacementTab::restart()
{
  topViewEvents_.clear();
  frontViewEvents_.clear();

  TrackMeTab::restart();
}

/**
 * @brief Refresh all plots.
 */
void DisplacementTab::replot()
{
  if (topViewEvents_.empty())
    return;

  auto baseTimestamp   = eventFeeder_->baseTimestamp();
  auto latestTimestamp = eventFeeder_->latestTimestamp();
  auto oldestTimestamp = latestTimestamp - PLOT_INTERVAL;

  displacementPlot_->setBaseTimestamp(baseTimestamp);
  displacementPlot_->setAxisScale(QwtPlot::xBottom,
                              oldestTimestamp, latestTimestamp);
  displacementPlot_->setSamples(topViewEvents_);

  topView_->setSamples(topViewEvents_);
  frontView_->setSamples(frontViewEvents_);
}

/**
 * @brief Export all plots to SVG.
 */
void DisplacementTab::exportPlots(const QString &path)
{
  displacementPlot_->exportSvg(path + "/displacement.svg");
  topView_->exportSvg(path + "/displacement_top_view.svg");
  frontView_->exportSvg(path + "/displacement_front_view.svg");
}
