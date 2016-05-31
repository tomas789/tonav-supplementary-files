#ifndef ORIENTATIONTAB_H
#define ORIENTATIONTAB_H

#include "trackmetab.h"

#include "../plots/sensorplot.h"
#include "../plots/fixedaspectratioplot.h"
#include <qwt_plot_curve.h>

#include <QList>

/**
 * @brief The OrientationTab class shows the orientation of the device
 *        from different views.
 */
class OrientationTab : public TrackMeTab
{
    Q_OBJECT
public:
    explicit OrientationTab(QWidget *parent = 0);

protected:
    SensorPlot *rotatedVector_;

    FixedAspectRatioPlot *topView_;
    QwtPlotCurve *topViewCurveX_;
    QwtPlotCurve *topViewCurveY_;
    QwtPlotCurve *topViewCurveZ_;

    FixedAspectRatioPlot *frontView_;
    QwtPlotCurve *frontViewCurveX_;
    QwtPlotCurve *frontViewCurveY_;
    QwtPlotCurve *frontViewCurveZ_;

    QList<PlotEvent> orientationEvents_;

public slots:
    void addOrientationEvent(const OrientationEvent &event);
    void restart() override;
    void replot() override;
    void exportPlots(const QString &path);
};

#endif // ORIENTATIONTAB_H
