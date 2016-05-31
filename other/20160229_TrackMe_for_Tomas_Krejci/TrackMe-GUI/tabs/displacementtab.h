#ifndef DISPLACEMENTTAB_H
#define DISPLACEMENTTAB_H

#include "trackmetab.h"

#include "../plots/pathplot.h"
#include "../plots/sensorplot.h"

#include <QList>

/**
 * @brief The DisplacementTab class plots the displacement of the device.
 */
class DisplacementTab : public TrackMeTab
{
    Q_OBJECT
public:
    explicit DisplacementTab(QWidget *parent = 0);

protected:
    SensorPlot *displacementPlot_;
    PathPlot   *topView_;
    PathPlot   *frontView_;
    QList<PlotEvent> topViewEvents_;
    QList<PlotEvent> frontViewEvents_;

public slots:
    void exportPlots(const QString &path);
    void addDisplacementEvent(const RawEvent &event);
    void restart() override;
    void replot() override;
};

#endif // DISPLACEMENTTAB_H
