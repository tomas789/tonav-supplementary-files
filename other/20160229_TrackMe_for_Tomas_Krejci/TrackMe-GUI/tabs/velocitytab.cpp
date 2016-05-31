#include "velocitytab.h"
#include "../utilities.h"

#include <QVBoxLayout>
#include <cmath>

VelocityTab::VelocityTab(QWidget *parent) :
  TrackMeTab(parent)
{
  auto layout = new QVBoxLayout;

  speedLabel_ = new QLabel;
  velocityPlot_ = new SensorPlot;
  velocityPlot_->setAxisTitle(QwtPlot::yLeft, "Value [m/s]");
  velocityPlot_->setAxisTitle(QwtPlot::xBottom, "Time [s.ms]");

  layout->addWidget(speedLabel_);
  layout->addWidget(velocityPlot_);

  setLayout(layout);
}

/**
 * @brief Slot for receiving new velocity sensor events.
 */
void VelocityTab::addVelocityEvent(const RawEvent &event)
{
  // Convert event types
  velocityEvents_.append(PlotEvent::from(event));

  // Calculate magnitude (speed)
  speed_ = 0;
  for (auto &v : event.value)
    speed_ += v * v;
  using std::sqrt;
  speed_ = sqrt(speed_);

  speedLabel_->setText(QString("Current speed: ") +
                       QString::number(speed_) +
                       QString(" m/s"));

  // Remove old events
  auto timestamp = eventFeeder_->latestTimestamp() - PLOT_INTERVAL;
  erase_old_events(velocityEvents_, timestamp);
}

/**
 * @brief Clear the plot and reinitialize.
 */
void VelocityTab::restart()
{
  velocityEvents_.clear();
  TrackMeTab::restart();
}

/**
 * @brief Refresh the plot.
 */
void VelocityTab::replot()
{
  if (velocityEvents_.empty())
    return;

  auto baseTimestamp   = eventFeeder_->baseTimestamp();
  auto latestTimestamp = eventFeeder_->latestTimestamp();
  auto oldestTimestamp = latestTimestamp - PLOT_INTERVAL;

  velocityPlot_->setBaseTimestamp(baseTimestamp);
  velocityPlot_->setAxisScale(QwtPlot::xBottom,
                           oldestTimestamp, latestTimestamp);
  velocityPlot_->setSamples(velocityEvents_);
}

/**
 * @brief Export the plot to a SVG file.
 */
void VelocityTab::exportPlots(const QString &path)
{
  velocityPlot_->exportSvg(path + "/velocity.svg");
}
