#include "sensorplot.h"

#include <qwt_plot_grid.h>
#include <qwt_plot_layout.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_engine.h>
#include <qwt_legend.h>
#include <qwt_plot_renderer.h>

#include <QString>

SensorPlot::SensorPlot(QWidget *parent) :
    QwtPlot(parent)
{
  setMinimumSize(300,100);

  setCanvasBackground(Qt::white);
  axisScaleEngine(QwtPlot::yLeft)->setAttribute(QwtScaleEngine::Symmetric);
  plotLayout()->setAlignCanvasToScales(true);

  /*
     Taken from qwt example cpuplot:
     In situations, when there is a label at the most right position of the
     scale, additional space is needed to display the overlapping part
     of the label would be taken by reducing the width of scale and canvas.
     To avoid this "jumping canvas" effect, we add a permanent margin.
     We don't need to do the same for the left border, because there
     is enough space for the overlapping label below the left scale.
   */
  QwtScaleWidget *scaleWidget = axisWidget(QwtPlot::xBottom);
  const int fmh = QFontMetrics(scaleWidget->font()).height();
  scaleWidget->setMinBorderDist(0, fmh / 2);

  // Avoid jumping when labels with more/less digits
  // appear/disappear when resizing horizontally
  const QFontMetrics fm(axisWidget(QwtPlot::yLeft )->font());
  QwtScaleDraw *sd = axisScaleDraw(QwtPlot::yLeft);
  sd->setMinimumExtent(fm.width("100.00"));

  timeScaleDraw_ = new TimeScaleDraw;
  setAxisScaleDraw(QwtPlot::xBottom, timeScaleDraw_);

  QwtLegend *legend = new QwtLegend;
  insertLegend(legend, QwtPlot::RightLegend);
  setAxisTitle(QwtPlot::xBottom, "Timestamp");
  setAxisTitle(QwtPlot::yLeft, "Value");

  QwtPlotGrid *grid = new QwtPlotGrid;
  grid->setPen(defaultGridPen());
  grid->attach(this);

  // Set a different color for each curve.
  for (int axis = 0; axis < 3; ++axis) {
      auto curve = curves_[axis] = new QwtPlotCurve;
      curve->setTitle("XYZ"[axis] + QString(" axis"));
      curve->setPen(defaultCurvePen(axis));
      curve->setRenderHint(QwtPlotItem::RenderAntialiased);

      curve->attach(this);
      curve->setVisible(true);
  }
}

/**
 * @brief Set the base timestamp the time labels are relative to.
 */
void SensorPlot::setBaseTimestamp(double baseTimestamp)
{
  timeScaleDraw_->setBaseTimestamp(baseTimestamp);
}

/**
 * @brief Export the plot to a SVG file.
 */
void SensorPlot::exportSvg(const QString &path) {
    QwtPlotRenderer renderer;
    renderer.renderDocument( this, path,
                             QSizeF( SVG_EXPORT_WIDTH, SVG_EXPORT_HEIGHT ) );
}

QPen SensorPlot::defaultCurvePen(int axis)
{
  QPen pen;
  static QColor colors[] = {Qt::green, Qt::blue, Qt::red};
  pen.setColor(colors[axis]);
  pen.setJoinStyle(Qt::RoundJoin);
  pen.setWidth(2);
  pen.setCosmetic(true);
  return pen;
}

QPen SensorPlot::defaultGridPen()
{
  QPen pen;
  pen.setColor(Qt::GlobalColor::lightGray);
  pen.setDashPattern({3, 5});
  pen.setWidth(1);
  pen.setCosmetic(true);
  return pen;
}

