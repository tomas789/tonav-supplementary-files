#include "infotab.h"

#include <QVBoxLayout>

InfoTab::InfoTab(QWidget *parent) :
  TrackMeTab(parent)
{
  auto layout = new QVBoxLayout;

  calibrated_ = new QLabel;
  layout->addWidget(calibrated_);

  layout->addWidget(new QLabel{"Accelerometer"});
  accStats_ = new StatsWidget;
  layout->addWidget(accStats_);
  layout->addWidget(new QLabel{"Gyroscope"});
  gyroStats_ = new StatsWidget;
  layout->addWidget(gyroStats_);
  layout->addWidget(new QLabel{"Magnetometer"});
  magStats_ = new StatsWidget;
  layout->addWidget(magStats_);

  this->setLayout(layout);
}

/**
 * @brief Refresh the statistics.
 */
void InfoTab::replot()
{
  if (eventFeeder_->calibrated()) {
    calibrated_->setStyleSheet("QLabel { color:green; }");
    calibrated_->setText("Successfully calibrated");
  } else {
    calibrated_->setStyleSheet("QLabel { color:red; }");
    calibrated_->setText("NOT CALIBRATED - hold the device still");
  }

  accStats_->assign(eventFeeder_->accelerometerStats());
  gyroStats_->assign(eventFeeder_->gyroscopeStats());
  magStats_->assign(eventFeeder_->magneticFieldStats());
}
