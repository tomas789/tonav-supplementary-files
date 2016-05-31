#include "fixedaspectratioplot.h"

#include <qwt_plot_canvas.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_renderer.h>

#include <algorithm>

FixedAspectRatioPlot::FixedAspectRatioPlot( QWidget *parent ) :
    QwtPlot(parent)
{
    std::fill(doAutoHint, doAutoHint + QwtPlot::axisCnt, true);
    std::fill(oldIntervals, oldIntervals + QwtPlot::axisCnt, QwtInterval());

    rescaler = new QwtPlotRescaler( this->canvas() );
    rescaler->setExpandingDirection( QwtPlotRescaler::ExpandBoth );
    rescaler->setRescalePolicy( QwtPlotRescaler::Fitting );
}

/**
 * @brief By default, all the axes will be scaled automatically, However,
 *        the user can choose an interval hint which alwas has to be
 *        visible.
 */
void FixedAspectRatioPlot::setIntervalHint( int axisId, QwtInterval axisInt )
{
    if ( axisValid( axisId ) ) {
        setAxisAutoHint( axisId, false );
        rescaler->setIntervalHint( axisId, axisInt );
    }
}

/**
 * @brief Return the current interval hint for an axis. Meaningless, if the
 *        axis is set to be scaled automatically.
 */
QwtInterval FixedAspectRatioPlot::intervalHint( int axisId )
{
    return rescaler->intervalHint( axisId );
}

/**
 * @brief Test if an axis is scaled automatically.
 */
bool FixedAspectRatioPlot::axisAutoHint( int axisId )
{
    if ( axisValid( axisId ) )
        return doAutoHint[axisId];
    else
        return false;
}

/**
 * @brief Set an axis to be scaled automatically and ignore its interval hint.
 */
void FixedAspectRatioPlot::setAxisAutoHint( int axisId, bool on )
{
    if ( axisValid( axisId ) )
        doAutoHint[axisId] = on;
}

/**
 * @brief The current aspect ratio of an axis. See the qwt rescaler.
 */
double FixedAspectRatioPlot::aspectRatio( int axisId ) const
{
    return rescaler->aspectRatio( axisId );
}

/**
 * @brief Set an aspect ratio of an axis. See the qwt rescaler.
 */
void FixedAspectRatioPlot::setAspectRatio( int axisId, double ratio )
{
    rescaler->setAspectRatio( axisId, ratio );
}

/**
 * @brief Iterates through all the visible plot items and sets the correct
 *        interval hints for all the axes with the automatic hint. Returns
 *        true if any scale has changed from the previous call. This value
 *        is futher used in the replot() to avoid infinite recursion.
 */
bool FixedAspectRatioPlot::adjustAxisIntervalHints()
{
    // Calculate axis interval of all visible plot items
    QwtInterval intv[QwtPlot::axisCnt];
    const QwtPlotItemList& itmList = itemList();

    for ( QwtPlotItemIterator it = itmList.begin(); it != itmList.end(); ++it ) {
        if ( (*it)->isVisible() ) {
            const QwtPlotItem *item = *it;
            const QRectF rect = item->boundingRect();

            if ( rect.width() >= 0.0 )
              intv[item->xAxis()] |= QwtInterval( rect.left(), rect.right() );

            if ( rect.height() >= 0.0 )
              intv[item->yAxis()] |= QwtInterval( rect.top(), rect.bottom() );
          }
    }

    // Set the interval hint for each axis that has axisAutoHint enabled
    bool intervalChanged = false;
    for ( int axisId = 0; axisId < QwtPlot::axisCnt; ++axisId ) {
        if ( intv[axisId].isValid() &&
             axisEnabled( axisId ) &&
             axisAutoHint( axisId ) &&
             oldIntervals[axisId] != intv[axisId]
        ) {
            intervalChanged = true;
            oldIntervals[axisId] = intv[axisId];
            rescaler->setIntervalHint( axisId, intv[axisId] );
        }
    }

    return intervalChanged;
}

/**
 * @brief If any axis interval changed, rescale and replot. Otherwise only
 *        replot.
 */
void FixedAspectRatioPlot::replot() {
    // Now rescale the canvas so it contains the entire interval hints
    bool intervalChanged = adjustAxisIntervalHints();
    if (intervalChanged)
        rescaler->rescale();
    else
        QwtPlot::replot();
}

/**
 * @brief Export the plot to a SVG file.
 */
void FixedAspectRatioPlot::exportSvg(const QString &path) {
    QwtPlotRenderer renderer;
    renderer.renderDocument( this, path, QSizeF(300,200) );
}
