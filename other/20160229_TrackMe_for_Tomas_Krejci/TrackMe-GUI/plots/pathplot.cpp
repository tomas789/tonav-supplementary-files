#include "pathplot.h"

PathPlot::PathPlot(QWidget *parent) :
    FixedAspectRatioPlot(parent)
{
    this->setCanvasBackground(Qt::white);
    this->setMinimumSize(300,100);

    background_curve_ = new QwtPlotCurve{};
    background_curve_->setPen(QPen(Qt::black));
    background_curve_->setRenderHint(QwtPlotItem::RenderAntialiased);
    background_curve_->attach(this);

    spectro_curve_ = new QwtPlotSpectroCurve{};
    spectro_curve_->setColorMap(new ColorMap{});
    spectro_curve_->setPenWidth(3);
    spectro_curve_->setRenderHint(QwtPlotItem::RenderAntialiased);
    spectro_curve_->attach(this);

    // Avoid jumping when labels with more/less digits
    // appear/disappear when resizing horizontally
    const QFontMetrics fm( axisWidget( QwtPlot::yLeft )->font() );
    axisScaleDraw( QwtPlot::yLeft  )->setMinimumExtent( fm.width( "100.00" ) );
    axisScaleDraw( QwtPlot::yRight )->setMinimumExtent( fm.width( "100.00" ) );

    // Let the rescaler ignore the height axis
    this->setAspectRatio(QwtPlot::yRight, 0.0);

    axisWidget(QwtPlot::xBottom)->setTitle("X coordinate");
    axisWidget(QwtPlot::yLeft)->setTitle("Y coordinate");
    axisWidget(QwtPlot::yRight)->setTitle("Z coordinate");

    this->enableAxis(QwtPlot::yRight);
}
