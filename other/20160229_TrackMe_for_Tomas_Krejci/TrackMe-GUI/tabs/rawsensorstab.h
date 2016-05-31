#ifndef RAWSENSORSTAB_H
#define RAWSENSORSTAB_H

#include "trackmetab.h"

#include "../plots/sensorplot.h"

#include <QList>

/**
 * @brief The RawSensorsTab class provides a tab for visualizing raw sensor
 *        events without any filters applied.
 */
class RawSensorsTab : public TrackMeTab
{
    Q_OBJECT
public:
    explicit RawSensorsTab(QWidget *parent = 0);

protected:
    SensorPlot *accelPlot_;
    SensorPlot *gyroPlot_;
    SensorPlot *magneticPlot_;
    QList<PlotEvent> accelEvents_;
    QList<PlotEvent> gyroEvents_;
    QList<PlotEvent> magneticEvents_;

public slots:
    void addRawEvent(const RawEvent &event);
    void restart() override;
    void replot() override;
    void exportPlots(const QString &path);
};

#endif // RAWSENSORSTAB_H
