#include "filteredsensorstab.h"

FilteredSensorsTab::FilteredSensorsTab(QWidget *parent) :
  RawSensorsTab(parent)
{
}

void FilteredSensorsTab::exportPlots(const QString &path)
{
  accelPlot_->exportSvg(path + "/filtered_accelerometer.svg");
  gyroPlot_->exportSvg(path + "/filtered_gyroscope.svg");
  magneticPlot_->exportSvg(path + "/filtered_magnetic_field.svg");
}
