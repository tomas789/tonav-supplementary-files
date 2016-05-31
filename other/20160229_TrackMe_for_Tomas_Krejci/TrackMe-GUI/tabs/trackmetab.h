#ifndef TRACKMETAB_H
#define TRACKMETAB_H

#include "../config.h"
#include "../eventfeeder.h"

#include <QWidget>
#include <QTimer>
#include <QTimerEvent>

/**
 * @brief The base class for all the tabs. Used for automatic updates
 *        of the plots on the tabs.
 */
class TrackMeTab : public QWidget
{
  Q_OBJECT
public:
  explicit TrackMeTab(QWidget *parent = 0);
  void setEventFeeder(const EventFeeder *eventFeeder);

protected:
  QTimer     timer_;
  const EventFeeder *eventFeeder_;

public slots:
  virtual void stop();
  virtual void restart();
  virtual void replot() = 0;
};

#endif // TRACKMETAB_H
