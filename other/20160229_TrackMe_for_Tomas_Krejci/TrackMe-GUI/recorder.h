#ifndef RECORDER_H
#define RECORDER_H

#include "config.h"
#include "eventfeeder.h"

#include <QObject>
#include <QString>
#include <QFile>

/**
 * @brief The Recorder class is used to record a communication with a client.
 *        It stores all the raw events to a file.
 */
class Recorder : public QObject
{
  Q_OBJECT
public:
  explicit Recorder(QObject *parent = 0);
  ~Recorder();
  void request(const EventFeeder *, const QString &path);
  bool recording() const;

public slots:
  void receiveRawEvent(const RawEvent &e);
  void receiveStartEvent();
  void receiveStopEvent();

private:
  const EventFeeder *eventFeeder_;
  void initFile();
  void closeFile();
  QFile file_;
  bool recording_ = false;
};

#endif // RECORDER_H
