#include "trackmetab.h"

TrackMeTab::TrackMeTab(QWidget *parent) :
  QWidget(parent)
{
  connect(&timer_, SIGNAL(timeout()), this, SLOT(replot()));
}

/**
 * @brief Stop timer.
 */
void TrackMeTab::stop()
{
  timer_.stop();
  timer_.singleShot(PLOT_REFRESH_INTERVAL, this, SLOT(replot()));
}

/**
 * @brief Restart timer.
 */
void TrackMeTab::restart()
{
  timer_.start(PLOT_REFRESH_INTERVAL);
}

/**
 * @brief Each tab owns a pointer to the event feeder. This function returns
 *        the pointer.
 */
void TrackMeTab::setEventFeeder(const EventFeeder *eventFeeder)
{
  this->eventFeeder_ = eventFeeder;
}
