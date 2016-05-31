#ifndef VELOCITYAB_H
#define VELOCITYAB_H

#include "trackmetab.h"

#include "../plots/sensorplot.h"

#include <QLabel>
#include <QList>

class VelocityTab : public TrackMeTab
{
    Q_OBJECT
public:
    explicit VelocityTab(QWidget *parent = 0);

protected:
    SensorPlot *velocityPlot_;
    QLabel *speedLabel_;
    PlotEvent::ValueT speed_;
    QList<PlotEvent> velocityEvents_;

public slots:
    void addVelocityEvent(const RawEvent &event);
    void restart() override;
    void replot() override;
    void exportPlots(const QString &path);
};

#endif // VELOCITYTAB_H
