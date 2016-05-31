#ifndef CONFIG_H
#define CONFIG_H

#include <AndroidEvent.hpp>
#include <TrackMe.hpp>

#include <QtGlobal>
#include <QMetaType>

// Register sensor events as Qt metatypes to be able to send them through
// the signals/slots mechanism.
typedef TrackMe::AndroidEvent RawEvent;
Q_DECLARE_METATYPE(RawEvent)
typedef typename TrackMe::Tracker<RawEvent>::OrientationEventT OrientationEvent;
Q_DECLARE_METATYPE(OrientationEvent)
typedef TrackMe::AxesEvent<qreal, qreal> PlotEvent;
Q_DECLARE_METATYPE(PlotEvent)

// Global configuration "constants".
extern Configuration<> CONFIG;
extern int PLOT_REFRESH_INTERVAL;
extern qreal PLOT_INTERVAL;
extern qreal SVG_EXPORT_WIDTH;
extern qreal SVG_EXPORT_HEIGHT;
extern int TCP_PORT;

void config();

#endif // CONFIG_H
