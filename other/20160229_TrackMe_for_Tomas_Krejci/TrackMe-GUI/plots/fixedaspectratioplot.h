#ifndef FIXEDASPECTRATIOPLOT_H
#define FIXEDASPECTRATIOPLOT_H

#include <qwt_interval.h>
#include <qwt_plot.h>
#include <qwt_plot_rescaler.h>

/**
 * @brief The FixedAspectRatioPlot class is a wrapper for qwt plot,
 *        which ensures that the data on the plot are are scaled in fixed aspect
 *        ratio. Useful, for instance, for path drawing with aspect 1:1.
 */
class FixedAspectRatioPlot : public QwtPlot
{
    Q_OBJECT

public:
    explicit FixedAspectRatioPlot(QWidget *parent = 0);
    QwtInterval intervalHint(int axisId);
    void setIntervalHint(int axisId, QwtInterval axisInt);
    bool axisAutoHint(int axisId);
    void setAxisAutoHint(int axisId, bool on = true);
    double aspectRatio(int axisId) const;
    void setAspectRatio(int axisId, double ratio);
    void exportSvg(const QString &path);

protected:
    bool adjustAxisIntervalHints();
    QwtPlotRescaler *rescaler;
    bool doAutoHint[QwtPlot::axisCnt];
    QwtInterval oldIntervals[QwtPlot::axisCnt];

public slots:
    virtual void replot();
};

#endif // FIXEDASPECTRATIOPLOT_H
