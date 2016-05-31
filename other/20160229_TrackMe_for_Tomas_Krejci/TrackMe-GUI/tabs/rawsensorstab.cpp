#include "rawsensorstab.h"
#include "../utilities.h"

#include "qwt_scale_engine.h"

#include <QVBoxLayout>

RawSensorsTab::RawSensorsTab(QWidget *parent) :
  TrackMeTab(parent)
{
  auto layout = new QVBoxLayout;

  accelPlot_ = new SensorPlot;
  gyroPlot_ = new SensorPlot;
  magneticPlot_ = new SensorPlot;

  accelPlot_->setTitle("Accelerometer");
  accelPlot_->setAxisTitle(QwtPlot::yLeft, "Value [m/s^2]");
  accelPlot_->setAxisTitle(QwtPlot::xBottom, "");
  gyroPlot_->setTitle("Gyroscope");
  gyroPlot_->setAxisTitle(QwtPlot::yLeft, "Value [rad/s]");
  gyroPlot_->setAxisTitle(QwtPlot::xBottom, "");
  magneticPlot_->setTitle("Magnetic field");
  magneticPlot_->setAxisTitle(QwtPlot::yLeft, "Value [microtesla]");
  magneticPlot_->setAxisTitle(QwtPlot::xBottom, "Time [s.ms]");

  layout->addWidget(accelPlot_);
  layout->addWidget(gyroPlot_);
  layout->addWidget(magneticPlot_);
  setLayout(layout);
}

/**
 * @brief Slot for receiving new raw sensor events.
 */
void RawSensorsTab::addRawEvent(const RawEvent &event)
{
  // Convert event types
  auto plotEvent = PlotEvent::from(event);

  switch (event.type) {
  case TrackMe::EventType::ACCELEROMETER:
    accelEvents_.append(plotEvent);
    break;
  case TrackMe::EventType::GYROSCOPE:
    gyroEvents_.append(plotEvent);
    break;
  case TrackMe::EventType::MAGNETIC_FIELD:
    magneticEvents_.append(plotEvent);
    break;
  default:
    return;
  }

  // Remove old events
  auto timestamp = eventFeeder_->latestTimestamp() - PLOT_INTERVAL;
  erase_old_events(accelEvents_, timestamp);
  erase_old_events(gyroEvents_, timestamp);
  erase_old_events(magneticEvents_, timestamp);
}

/**
 * @brief Clear the plots and reinitialize.
 */
void RawSensorsTab::restart()
{
  accelEvents_.clear();
  gyroEvents_.clear();
  magneticEvents_.clear();

  TrackMeTab::restart();
}

/**
 * @brief Refresh the plots.
 */
void RawSensorsTab::replot()
{
  auto baseTimestamp   = eventFeeder_->baseTimestamp();
  auto latestTimestamp = eventFeeder_->latestTimestamp();
  auto oldestTimestamp = latestTimestamp - PLOT_INTERVAL;

  accelPlot_->setBaseTimestamp(baseTimestamp);
  accelPlot_->setAxisScale(QwtPlot::xBottom,
                           oldestTimestamp, latestTimestamp);
  accelPlot_->setSamples(accelEvents_);

  gyroPlot_->setBaseTimestamp(baseTimestamp);
  gyroPlot_->setAxisScale(QwtPlot::xBottom,
                          oldestTimestamp, latestTimestamp);
  gyroPlot_->setSamples(gyroEvents_);

  magneticPlot_->setBaseTimestamp(baseTimestamp);
  magneticPlot_->setAxisScale(QwtPlot::xBottom,
                              oldestTimestamp, latestTimestamp);
  magneticPlot_->setSamples(magneticEvents_);
}

void RawSensorsTab::exportPlots(const QString &path)
{
  accelPlot_->exportSvg(path + "/raw_accelerometer.svg");
  gyroPlot_->exportSvg(path + "/raw_gyroscope.svg");
  magneticPlot_->exportSvg(path + "/raw_magnetic_field.svg");
}
